// board[kolumna][rząd]
// Kolumna: A,B,C..., jest enum, więc można w kodzie używać literek
// Rząd: 1,2,3..., UWAGA! w kodzie również numerowane są od 1!
#include "gra.h"

int abs(int number) { // Zwraca wartość bezwględną liczby
    if (number < 0)
        return -number;
    return number;
}

enum COLOR get_color(enum FIELD_STATE board[8][9], int col, int row) { // Jakiego koloru jest podana figura
    if (board[col][row] == RED_PAWN || board[col][row] == RED_KING) return RED;
    else if (board[col][row] == WHITE_PAWN || board[col][row] == WHITE_KING) return WHITE;
    else return NO_COLOR;
}

enum TYPE get_type(enum FIELD_STATE board[8][9], int col, int row) { // Jakiego typu jest figura
    if(board[col][row] == RED_PAWN || board[col][row] == WHITE_PAWN) return PAWN;
    else return KING;
}

enum BOOL is_enemy(enum FIELD_STATE board[8][9], int col, int row, enum COLOR your_color) { // Czy na podanym polu jest przeciwnik
    enum COLOR enemy_color = get_color(board, col, row);
    if (enemy_color != your_color && enemy_color != NO_COLOR)
        return TRUE;
    else 
        return FALSE;
}

void generate_board(enum FIELD_STATE board[8][9], enum GAME_STATE state, char *board_string) {  // Generuje podgląd aktualnego rozkładu pionków
    strcpy(board_string, "STAN GRY:\n");
    char tmp_string[50];
    switch(state) {
        case NEW_GAME:   strcat(board_string, "Nowa gra\n");          break;
        case WHITE_TURN: strcat(board_string, "Tura bialych\n");      break;
        case RED_TURN:   strcat(board_string, "Tura czerwonych\n");   break;
        case WHITE_WIN:  strcat(board_string, "Bialy wygrywa!\n");    break;
        case RED_WIN:    strcat(board_string, "Czerwony wygrywa!\n"); break;
        default: sprintf(tmp_string, "%d\n", state); strcat(board_string, tmp_string); break;
    }
    strcat(board_string, "   ");
    for (char row='A'; row<='H'; ++row) {
        sprintf(tmp_string, "%c   ", row);
        strcat(board_string, tmp_string);
    }
    strcat(board_string, "\n ╔");
    for (int i=0; i<7; ++i)
            strcat(board_string, "═══╤");
    strcat(board_string, "═══╗\n");
    for (int row=1; row<=8; ++row) {
        sprintf(tmp_string, "%d║", row);
        strcat(board_string, tmp_string);
        for (int col=A; col<=H; ++col) {
                switch(board[col][row]) {
                    case FREE:       strcpy(tmp_string, " "); break;
                    case WHITE_PAWN: strcpy(tmp_string, "W"); break;
                    case WHITE_KING: strcpy(tmp_string, "M"); break;
                    case RED_PAWN:   strcpy(tmp_string, "R"); break;
                    case RED_KING:   strcpy(tmp_string, "P"); break;
                    default: sprintf(tmp_string, "%d", board[col][row]); break;
                }
            strcat(board_string, " ");
            strcat(board_string, tmp_string);
            if (col != H) strcat(board_string, " │");
            else strcat(board_string, " ");
        } 
        sprintf(tmp_string, "║%d", row);
        strcat(board_string, tmp_string);
        if (row != 8) {
            strcat(board_string, "\n ╟");
            for (int i=0; i<7; ++i)
                strcat(board_string, "───┼");
            strcat(board_string, "───╢\n");
        }
    }
    strcat(board_string, "\n ╚");
    for (int i=0; i<7; ++i)
            strcat(board_string, "═══╧");
    strcat(board_string, "═══╝\n   ");
    for (char row='A'; row<='H'; ++row) {
        sprintf(tmp_string, "%c   ", row);
        strcat(board_string, tmp_string);
    }
    strcat(board_string, "\n\0");
}

void generate_error_message(enum MOVE_ERROR move_error_no, char* message) { // Generuj wiadomość o błędzie
    switch(move_error_no) {
        case NO_ERROR:             break;
        case ERROR_WRONG_STATE:    strcpy(message, "Nieprawidlowy stan gry!\n");                 break;
        case ERROR_FIELD_INVALID:  strcpy(message, "Nieprawidlowy indeks pola!\n");              break;
        case ERROR_FIELD_TAKEN:    strcpy(message, "Pole docelowe zajete!\n");                   break;
        case ERROR_WRONG_FIGURE:   strcpy(message, "Nie twoja figura!/Nie ma takiej figury!\n"); break;
        case ERROR_FORCE_CAPTURE:  strcpy(message, "Masz mozliwe bicie, wiec musisz bic!\n");    break;
        case ERROR_NO_ENEMY:       strcpy(message, "Na twojej drodze nie ma przeciwnika!\n");    break;
        case ERROR_MOVE_INCORRECT: strcpy(message, "Nieprawidlowy ruch!\n");                     break;
        default:                   sprintf(message, "Error no. %d", move_error_no);   break;
    }
}

void new_game(enum FIELD_STATE board[8][9], enum GAME_STATE *state) { // Rozpoczyna nową grę
    // Ustaw piony na planszy
    for (int col=A; col<=H; col+=2) {
        board[col][1] = RED_PAWN;
        board[col][3] = RED_PAWN;
        board[col][7] = WHITE_PAWN;
    }

    for (int col=B; col<=H; col+=2) {
        board[col][6] = WHITE_PAWN;
        board[col][8] = WHITE_PAWN;
        board[col][2] = RED_PAWN;
    }
    *state = WHITE_TURN; // Ustaw turę białych
        
}

enum BOOL can_move(enum FIELD_STATE board[8][9], int col, int row) { // Sprawdzenie czy figura może się poruszyć
    int row_direction = get_color(board, col, row); // Kolor decyduje o tym, czy pionek może poruszać się w górę czy w dół rzędów
    for (int col_direction=-1; col_direction<=1; col_direction+=2) { // Sprawdzamy możliwość ruchu w lewo lub w prawo
        // Sprawdzamy możliwość ruchu odpowiednio w górę albo w dół (z perspektywy piona w przód)
        if (col+col_direction >= A && col+col_direction <= H) { // Jeśli kolumna docelowa jest poprawna
            if (row+row_direction>=1 && row+row_direction<=8 && board[col+col_direction][row+row_direction] == FREE) { // Jeśli pole docelowe jest puste
                return TRUE;
            }
            if (row-row_direction>=1 && row-row_direction<=8 && get_type(board, col, row) == KING) { // Jeśli figura jest królem, to sprawdzamy możliwość ruchu w tył
                if (board[col+col_direction][row-row_direction] == FREE) { // Jeśli pole docelowe jest puste
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

enum BOOL can_capture(enum FIELD_STATE board[8][9], int col, int row) { // Sprawdzanie czy figura może coś zbić
    // Biały pionek może zmniejszyć swój rząd, czerwony może zwiększyć, król może jedno albo drugie
    int dest_col;
    int dest_row;
    int enemy_col;
    int enemy_row;
    enum COLOR your_color = get_color(board, col, row);
    for (int direction=-1; direction<=1; direction+=2) { // Dla -1 sprawdzamy czy jest bicie po lewej, dla +1 czy jest bicie po prawej
        enemy_col = col + direction;
        enemy_row = row + your_color; // Dla białego rząd w dół, dla czerwonego rząd w górę
        dest_col = col + 2*direction;
        dest_row = row + 2*your_color;
        if (dest_col>=0 && dest_col<=H && dest_row>=1 && dest_row<=8) {
            if (is_enemy(board, enemy_col, enemy_row, your_color) && board[dest_col][dest_row] == FREE) { // Jeśli na sąsiednim polu jest przeciwnik, a za nim jest wolne miejsce
                //printf("Za pomoca %c%d mozesz zbic %c%d i wyladowac na %c%d\n", col+65, row, enemy_col+65, enemy_row, dest_col+65, dest_row);
                return TRUE;
            }
            if (get_type(board, col, row) == KING) { // Jeśli figura jest królem, to może bić w tył
                enemy_row = row - your_color; // Sprawdzamy opcję przeciwną do tej sprawdzonej dla zwykłego pionka
                dest_row = row - 2*your_color;
                if (is_enemy(board, enemy_col, enemy_row, your_color) && board[dest_col][dest_row] == FREE) {
                    //printf("Za pomoca %c%d mozesz zbic %c%d i wyladowac na %c%d\n", col+65, row, enemy_col+65, enemy_row, dest_col+65, dest_row);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

// Porusznie figurą
enum MOVE_ERROR move(enum FIELD_STATE board[8][9], enum GAME_STATE *state, int last_used_figure[3], int from_col, int from_row, int where_col, int where_row) {
    enum COLOR current_player_color;
    // Ustalanie czyja tura teraz trwa
    switch(*state) {
        case WHITE_TURN: current_player_color = WHITE; break;
        case RED_TURN: current_player_color = RED; break;
        default: return ERROR_WRONG_STATE;
    }
    // Sprawdzenie czy początkowe pole jest poprawne
    if (from_col<A || from_col>H || from_row<1 || from_row>8)
        return ERROR_FIELD_INVALID;
    // Sprawdzenie czy docelowe pole jest poprawne
    if (where_col<A || where_col>H || where_row<1 || where_row>8)
        return ERROR_FIELD_INVALID;
    // Sprawdzanie czy docelowe pole jest puste
    if (board[where_col][where_row] != FREE)
        return ERROR_FIELD_TAKEN;
    // Sprawdzenie czy gracz podniósł własną figurę
    if (current_player_color != get_color(board, from_col, from_row))
        return ERROR_WRONG_FIGURE;
    // Jeśli w poprzednim ruchu gracz nie skończył serii bicia, musi ją kontynuować
    if (last_used_figure[0] == TRUE) {
        // Jeśli gracz próbuje poruszyć się o 2 pola to znaczy, że próbuje coś zbić
        if (last_used_figure[1] == from_col && last_used_figure[2] == from_row && abs(where_col-from_col) == 2)
            last_used_figure[0] = FALSE;
        else
            return ERROR_FORCE_CAPTURE;
    }

    // Normalny ruch
    int row_direction = current_player_color; // Kolor decyduje o tym, czy pionek może poruszać się w górę, czy w dół rzędów
    if (abs(where_col-from_col) == 1) { // Ruch o jedno pole w poziomie
        // "Słaby" przymus bicia. Wiem, że mieliśmy tego nie robić, ale i tak musiałbym to zaimplementować przy serii bicia.
        // Przymus "słaby", bo nie sprawdzam możliwości bicia dla wszystkich pionków gracza, a jedynie tego, który został podniesiony
        if (can_capture(board, from_col, from_row))
            return ERROR_FORCE_CAPTURE;
        // Ruch o jedno pole w odpowiednią dla koloru stronę, lub jeśli jest królem również w przeciwną stronę
        if ((where_row-from_row == row_direction) || (get_type(board, from_col, from_row) == KING && where_row-from_row == -row_direction)) { 
            board[where_col][where_row] = board[from_col][from_row];
            board[from_col][from_row] = FREE;
            if (*state == WHITE_TURN) { // W zależności od koloru gracza
                if (where_row == 1) // Jeśli dotarł na koniec, awansuj na króla
                    board[where_col][where_row] = WHITE_KING;
                *state = RED_TURN; // Oddaj turę
            }
            else {
                if (where_row == 8) // Jeśli dotarł na koniec, awansuj na króla
                    board[where_col][where_row] = RED_KING;
                *state = WHITE_TURN; // Oddaj turę
            }
            return NO_ERROR;
        }
        else return ERROR_MOVE_INCORRECT;
    }
    // Bicie
    else if (abs(where_col-from_col) == 2) { // Ruch od 2 pola w poziomie
        // Analogicznie jak przy zwykłym ruchu, jednak tym razem ruch o 2 pola
        if (where_row-from_row == 2*row_direction) { // Dla piona
            // Sprawdzanie czy po drodze jest przeciwnik
            int enemy_col = from_col + (where_col > from_col ? 1:-1); // Położenie przeciwnika zależy od tego czy bijemy w prawo, czy w lewo
            int enemy_row = from_row + row_direction; // Bicie w górę lub w dół zależy od koloru gracza
            if (is_enemy(board, enemy_col, enemy_row, current_player_color)) { // Między polem startowym, a docelowym jest przeciwnik
                board[where_col][where_row] = board[from_col][from_row];
                board[from_col][from_row] = FREE;
                board[enemy_col][enemy_row] = FREE;
                // Jeśli nie może więcej bić, oddaj turę
                if (can_capture(board, where_col, where_row) == FALSE) {
                    if (*state == WHITE_TURN) { // W zależności od koloru gracza
                        if (where_row == 1) // Jeśli dotarł na koniec, awansuj na króla
                            board[where_col][where_row] = WHITE_KING;
                        *state = RED_TURN; // Oddaj turę
                    }
                    else {
                        if (where_row == 8) // Jeśli dotarł na koniec, awansuj na króla
                            board[where_col][where_row] = RED_KING;
                        *state = WHITE_TURN; // Oddaj turę
                    }
                }
                else { // Jeśli jednak może coś zbić, zmuś gracza do użycia następnym razem tego samego pionka
                    last_used_figure[0] = TRUE;
                    last_used_figure[1] = where_col;
                    last_used_figure[2] = where_row;
                }
                return NO_ERROR;
            }
        }
        else if (get_type(board, from_col, from_row) == KING && (abs(where_row-from_row) == 2)) { // Dla króla jeszcze sprawdzenie w przeciwną stronę
            int enemy_col = from_col + (where_col > from_col ? 1:-1); // Położenie przeciwnika zależy od tego czy bijemy w prawo, czy w lewo
            int enemy_row;
            if (from_row-where_row == 2) 
                enemy_row = from_row - 1; // Ruch w górę
            else
                enemy_row = from_row + 1; // Ruch w dół
            if (is_enemy(board, enemy_col, enemy_row, current_player_color)) { // Między polem startowym, a docelowym jest przeciwnik
                board[where_col][where_row] = board[from_col][from_row];
                board[from_col][from_row] = FREE;
                board[enemy_col][enemy_row] = FREE;
                // Jeśli nie może więcej bić, oddaj turę
                if (can_capture(board, where_col, where_row) == FALSE) {
                    if (*state == WHITE_TURN) *state = RED_TURN;
                    else *state = WHITE_TURN;
                }
                else { // Jeśli jednak może coś zbić, zmuś gracza do użycia następnym razem tego samego pionka
                    last_used_figure[0] = TRUE;
                    last_used_figure[1] = where_col;
                    last_used_figure[2] = where_row;
                }
                return NO_ERROR;
            } else return ERROR_NO_ENEMY;
        }
        else return ERROR_MOVE_INCORRECT;
    }
    else return ERROR_MOVE_INCORRECT;
return ERROR_MOVE_INCORRECT;
}
  //TODO: Co jeśli obaj gracze się zablokują
enum COLOR is_win(enum FIELD_STATE board[8][9], enum GAME_STATE *state) { // Czy nastąpiło zwycięstwo, zwraca kolor zwycięzcy lub NO_COLOR
    // Ilość pionków pozostała na planszy i mogą się poruszyć/bić
    int white_figures = 0;
    int red_figures = 0;
    enum COLOR field_color; // Kolor figury na danym polu
    for (int col=A; col<=H; ++col) {
        for (int row=1; row<=8; ++row) {
            field_color = get_color(board, col, row);
            if (field_color == RED) {
                if (can_move(board, col, row) || can_capture(board, col, row)) { // Jeśli figura może się poruszyć lub bić
                    red_figures += 1;
                }
            }
            else if (field_color == WHITE) {
                if (can_move(board, col, row) || can_capture(board, col, row)) { // Jeśli figura może się poruszyć lub bić
                    white_figures += 1;
                }
            }
        }
    }
    //printf("Czerwone: %d, Biale: %d\n", red_figures, white_figures);
    // Ogłoszenie zwycięzcy
    if (white_figures == 0) {
        *state = RED_WIN;
        return RED;
    }
    else if (red_figures == 0) {
        *state = WHITE_WIN;
        return WHITE;
    }
    else
        return NO_COLOR;
}

void print(int a){
    if(write(1,&a,sizeof(a))<0){
        perror("write");exit(1);
    }
}
void print_str(char tab[], int size){
    if(write(1,tab,size)<0){
        perror("write_str");exit(1);
    }
}
void scan(char *tab, int size){
    int i = 0,n;
    char pom=0;
    while((n=read(0,&pom,1))>0 && pom!='\n'){
        if(i<size)tab[i++]=pom;
        if( pom=='\n' || pom=='\0')break;
    }
    if(n<0){
        perror("read");exit(1);
    }
}

int main() {
    enum FIELD_STATE board[8][9] = {FREE}; // Tablilca przechowująca planszę do gry, rząd 0 jest pusta, żeby móc używać oznaczeń z normalnej planszy
    enum GAME_STATE state = NEW_GAME; // Aktualny stan gry
    int last_used_figure[3] = {0, 0, 0}; // Czy ostatio użyta figura musi być znowu użyta i gdzie znajduje się ona znajduje: przymus, kolumna, rząd
    enum MOVE_ERROR move_error_no = NO_ERROR;
    char board_string[1500];
    char error_message[50];
    new_game(board, &state);
    generate_board(board, state, board_string);
    print(move_error_no);
    print(state);
    print(sizeof(board_string));
    print_str(board_string, sizeof(board_string));
    while (1) {
        char input[6];
        scan(input,6);
        int from_col = (int)input[0]-65;
        int from_row = (int)input[1]-48;
        int where_col = (int)input[3]-65;
        int where_row = (int)input[4]-48;
        if (from_col>=A && from_col<=H  && from_row>=1 && from_row<=8 && where_col>=A && where_col<=H && where_row>=1 && where_row<=8) {
            move_error_no = move(board, &state, last_used_figure, from_col, from_row, where_col, where_row);
            print(move_error_no);
            if (move_error_no == 0) {
                is_win(board, &state);
                generate_board(board, state, board_string);
                print(state);
                print(sizeof(board_string));
                print_str(board_string,sizeof(board_string));
            }
            else {
                generate_error_message(move_error_no, error_message);
                print(state);
                print(sizeof(error_message));
                print_str(error_message, sizeof(error_message));
            }
        }
        else {
            print(-1);
            print(state);
            print(sizeof("Input error!, Używaj formatu A1<spacja>C2\n"));
            print_str("Input error!, Używaj formatu A1<spacja>C2\n", sizeof("Input error!, Używaj formatu A1<spacja>C2\n"));
        }

    }
    return 0;
}
