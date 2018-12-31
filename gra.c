// board[kolumna][rząd]
// Kolumna: A,B,C..., jest enum, więc można w kodzie używać literek
// Rząd: 1,2,3..., UWAGA! w kodzie również numerowane są od 1!
#include <stdio.h>
#include <string.h>

enum COL {A, B, C, D, E, F, G, H};
enum GAME_STATE {NEW_GAME, WHITE_TURN, RED_TURN, WHITE_WIN, RED_WIN}; // Nie wiem, czy wszystkie się przydadzą
enum FIELD_STATE {FREE, WHITE_PAWN, WHITE_KING, RED_PAWN, RED_KING}; // Możliwe stany dla każdego z pól
enum MOVE_ERROR {NO_ERROR, ERROR_WRONG_STATE, ERROR_FIELD_INVALID, ERROR_FIELD_TAKEN, ERROR_WRONG_FIGURE, ERROR_FORCE_CAPTURE, ERROR_NO_ENEMY, ERROR_MOVE_INCORRECT};
enum BOOL {FALSE, TRUE}; // W C nie ma boola, więc go dodaję
enum COLOR {WHITE=-1, NO_COLOR, RED}; // Kolor, pomaga w ustaleniu dozwolonego kierunku ruchu pionka
enum TYPE {PAWN, KING}; // Typ figury

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


void print_board(enum FIELD_STATE board[8][9], enum GAME_STATE state) {  // Wyswietla aktualny rozkład pionków
    char board_string[512] = "STAN GRY:\n";
    char tmp_string[50];
    switch(state) {
        case NEW_GAME: strcat(board_string, "Nowa gra\n"); break;
        case WHITE_TURN: strcat(board_string, "Tura bialych\n"); break;
        case RED_TURN: strcat(board_string, "Tura czerwonych\n"); break;
        default: sprintf(tmp_string, "%d\n", state); strcat(board_string, tmp_string); break;
    }
    strcat(board_string, "  ");
    for (char row='A'; row<='H'; ++row) {
        sprintf(tmp_string, "%c ", row);
        strcat(board_string, tmp_string);
    }
    strcat(board_string, "\n  ───────────────\n");
    for (int row=1; row<=8; ++row) {
        sprintf(tmp_string, "%d│", row);
        strcat(board_string, tmp_string);
        for (int col=A; col<=H; ++col)
            switch(board[col][row]) {
                case FREE: strcat(board_string, "  "); break;
                case WHITE_PAWN: strcat(board_string, "W "); break;
                case WHITE_KING: strcat(board_string, "M "); break;
                case RED_PAWN: strcat(board_string, "R "); break;
                case RED_KING: strcat(board_string, "P "); break;
                default: sprintf(tmp_string, "%d ", board[col][row]); strcat(board_string, tmp_string); break;
            }
         sprintf(tmp_string, "|%d\n", row);
         strcat(board_string, tmp_string);
    }
    strcat(board_string, "  ───────────────\n  ");
    for (char row='A'; row<='H'; ++row) {
        sprintf(tmp_string, "%c ", row);
        strcat(board_string, tmp_string);
    }
    strcat(board_string, "\n");
    printf("%s\n", board_string);
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

enum BOOL can_capture(enum FIELD_STATE board[8][9], int col, int row) { // Sprawdzanie czy pionek może coś zbić
    // Biały pionek może zmniejszyć swój rząd, czerwony może zwiększyć, król może jedno albo drugie
    int dest_col;
    int dest_row;
    int enemy_col;
    int enemy_row;
    enum COLOR your_color = get_color(board, col, row);
    if (your_color == NO_COLOR) // Jeśli gracz wybierze nieodpowiednie pole docelowe, to z powodu takiej, a nie innej kolejności obsługi błędów może tu trafić, więc trzeba go puścić dalej
        return FALSE;
    enum COLOR enemy_color = your_color==WHITE ? RED:WHITE; // Wybierz kolor przciwnika
    for (int direction=-1; direction<=1; direction+=2) { // Dla -1 sprawdzamy czy jest bicie po lewej, dla +1 czy jest bicie po prawej
        enemy_col = col + direction;
        enemy_row = row + your_color; // Dla białego rząd w dół, dla czerwonego rząd w górę
        dest_col = col + 2*direction;
        dest_row = row + 2*your_color;
        if (dest_col>=0 && dest_col<=H && dest_row>=1 && dest_row<=8) {
            if (get_color(board,enemy_col, enemy_row) == enemy_color && board[dest_col][dest_row] == FREE) { // Jeśli na sąsiednim polu jest przeciwnik, a za nim jest wolne miejsce
                printf("Za pomoca %c%d mozesz zbic %c%d i wyladowac na %c%d\n", col+65, row, enemy_col+65, enemy_row, dest_col+65, dest_row);
                return TRUE;
            }
            if (get_type(board, col, row) == KING) { // Jeśli figura jest królem, to może bić w tył
                enemy_row = row - your_color; // Sprawdzamy opcję przeciwną do tej sprawdzonej dla zwykłego pionka
                dest_row = row - your_color;
                if (get_color(board, enemy_col, enemy_row) == enemy_color && board[dest_col][dest_row] == FREE) {
                    printf("Za pomoca %c%d mozesz zbic %c%d i wyladowac na %c%d\n", col+65, row, enemy_col+65, enemy_row, dest_col+65, dest_row);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

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
    if (last_used_figure[0] == TRUE && last_used_figure[1] == from_row && last_used_figure[2] == from_col) {
        // Jeśli gracz próbuje poruszyć się o 2 pola to znaczy, że próbuje coś zbić
        if (abs(where_col-from_col) == 2)
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
        if (where_row-from_row == 2*row_direction || (get_type(board, from_col, from_row) == KING && where_row-from_row == -2*row_direction)) {
            // Sprawdzanie czy po drodze jest przeciwnik
            int enemy_col_modifier = (where_col > from_col ? 1:-1);
            int enemy_col = from_col + enemy_col_modifier; // Położenie przeciwnika zależy od tego czy bijemy w prawo, czy w lewo
            int enemy_row = from_row + row_direction; // Bicie w górę lub w dół zależy od koloru gracza
            if (get_color(board, enemy_col, enemy_row) != current_player_color) { // Między polem startowym, a docelowym jest przeciwnik
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
            else if (get_type(board, from_col, from_row) == KING) { // Dla króla jeszcze sprawdzenie w przeciwną stronę
                enemy_col = from_col - enemy_col_modifier;
                if (get_color(board, enemy_col, enemy_row) != current_player_color) { // Między polem startowym, a docelowym jest przeciwnik
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
    }
    else return ERROR_MOVE_INCORRECT;
}

int main() {
    // !!!Oczywiście, wszystko w main służy tylko do testów i ostatecznie przyjmie inną formę!!!
    enum FIELD_STATE board[8][9] = {FREE}; // Tablilca przechowująca planszę do gry, rząd 0 jest pusta, żeby móc używać oznaczeń z normalnej planszy
    enum GAME_STATE state = NEW_GAME; // Aktualny stan gry
    int last_used_figure[3] = {0, 0, 0}; // Czy ostatio użyta figura musi być znowu użyta i gdzie znajduje się ona znajduje: przymus, kolumna, rząd
    enum MOVE_ERROR move_error_no;
    new_game(board, &state);
    
    /*state = RED_TURN;
    for (int col=A; col<=H; ++col)
        for (int row=1; row<=8; ++row)
            board[col][row] = FREE;
    board[F][4] = RED_PAWN;
    board[E][5] = WHITE_PAWN;
    board[C][7] = WHITE_PAWN;
    board[H][8] = WHITE_PAWN;
    board[F][8] = WHITE_PAWN;*/

    while (1) {
        print_board(board, state);
        char input[6];
        fgets(input, 6, stdin); // A1<spacja>B
        int from_col = (int)input[0]-65;
        int from_row = (int)input[1]-48;
        int where_col = (int)input[3]-65;
        int where_row = (int)input[4]-48;
        if (from_col>=A && from_col<=H  && from_row>=1 && from_row<=8 && where_col>=A && where_col<=H && where_row>=1 && where_row<=8) {
            //printf("%c%d %c%d\n", from_col+65, from_row, where_col+65, where_row);
            if ((move_error_no = move(board, &state, last_used_figure, from_col, from_row, where_col, where_row)) != 0) {
                switch(move_error_no) {
                    case ERROR_WRONG_STATE: printf("Nieprawidlowy stan gry!\n"); break;
                    case ERROR_FIELD_INVALID: printf("Nieprawidlowy indeks pola!\n"); break;
                    case ERROR_FIELD_TAKEN: printf("Pole docelowe zajete!\n"); break;
                    case ERROR_WRONG_FIGURE: printf("Nie twoja figura!/Nie ma takiej figury!\n"); break;
                    case ERROR_FORCE_CAPTURE: printf("Masz mozliwe bicie, wiec musisz bic!\n"); break;
                    case ERROR_NO_ENEMY: printf("Na twojej drodze nie ma przeciwnika!\n"); break;
                    case ERROR_MOVE_INCORRECT: printf("Nieprawidlowy ruch!\n"); break;
                    default: printf("Error no. %d", move_error_no); break;
                }
            }
        }
        else {
            printf("Input error!, Używaj formatu A1<spacja>C2\n");
        }
        fgets(input, 6, stdin);
    }
    return 0;
}
