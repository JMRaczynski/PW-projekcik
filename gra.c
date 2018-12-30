// board[kolumna][rząd]
// Kolumna: A,B,C..., jest enum, więc można w kodzie używać literek
// Rząd: 1,2,3..., UWAGA! w kodzie również numerowane są od 1!
#include <stdio.h>
#include <string.h>

enum COL {A, B, C, D, E, F, G, H};
enum GAME_STATE {NEW_GAME, WHITE_TURN, RED_TURN, WHITE_WIN, RED_WIN}; // Nie wiem, czy wszystkie się przydadzą
enum FIELD_STATE {FREE, WHITE_PAWN, WHITE_KING, RED_PAWN, RED_KING}; // Możliwe stany dla każdego z pól
enum BOOL {FALSE, TRUE}; // W C nie ma boola, więc go dodaję

int abs(int number) { // Zwraca wartość bezwględną liczby
    if (number < 0)
        return -number;
    return number;
}

void print_board(enum FIELD_STATE board[8][9], enum GAME_STATE state) {  // Wyswietla aktualny rozkład pionków
    printf("STAN GRY:\n");
    switch(state) {
        case NEW_GAME: printf("Nowa gra\n"); break;
        case WHITE_TURN: printf("Tura bialych\n"); break;
        case RED_TURN: printf("Tura czerwonych\n"); break;
        default: printf("%d\n", state); break;
    }
    printf("  ");
    for (char row='A'; row<='H'; ++row)
        printf("%c ", row);
    printf("\n  ───────────────\n");
    for (int row=1; row<=8; ++row) {
        printf("%d│", row);
        for (int col=A; col<=H; ++col)
            switch(board[col][row]) {
                case FREE: printf("  "); break;
                case WHITE_PAWN: printf("W "); break;
                case RED_PAWN: printf("R "); break;
                //TODO: Dodać oznaczenia dla królów
                default: printf("%d ", board[col][row]); break;
            }
         printf("|%d\n", row);
    }
    printf("  ───────────────\n  ");
    for (char row='A'; row<='H'; ++row)
        printf("%c ", row);
    printf("\n");
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

enum BOOL move(enum FIELD_STATE board[8][9], enum GAME_STATE *state, int from_col, int from_row, int where_col, int where_row) { // TODO: Wymyślić jakim typem danych mają być współrzędne
    //TODO: Przeslac komunikaty o bledzie do klienta
    // Sprawdzanie czy pole docelowe jest poprawne
    if (where_col < 0 || where_col > 8 || where_row < 1 || where_row > 9) {
        printf("Invalid destination field!\n");
        return FALSE;
    }
    // Sprawdzanie czy pole docelowe jest wolne
    if (board[where_col][where_row] != FREE) {
        printf("Destination field is taken!\n");
        return FALSE;
    }
    // Jeśli pionek białego gracza
    if (*state == WHITE_TURN && board[from_col][from_row] == WHITE_PAWN) {
        if ((abs(where_col-from_col) == 1) && (where_row-from_row == -1)) {
            board[where_col][where_row] = board[from_col][from_row];
            board[from_col][from_row] = 0;
            // Przekazanie tury drugiemu graczowi
            *state = RED_TURN;
            return TRUE;
        }
        // TODO: Dodać bicie jako else if
        else {
            printf("Incorrect move!\n");
            return FALSE;
        }
    }
    // Jeśli pionek czerwonego gracza
    if (*state == RED_TURN && board[from_col][from_row] == RED_PAWN) {
        if ((abs(where_col-from_col) == 1) && (where_row-from_row == 1)) {
            board[where_col][where_row] = board[from_col][from_row];
            board[from_col][from_row] = 0;
            // Przekazanie tury drugiemu graczowi
            *state = WHITE_TURN;
            return TRUE;
        }
        // TODO: Dodać bicie jako else if
        else {
            printf("Incorrect move!\n");
            return FALSE;
        }
    }
    // Jeśli król, dowolnego koloru
    if ((*state == WHITE_TURN && board[from_col][from_row] == WHITE_KING) || (*state == RED_TURN && board[from_col][from_row] == RED_KING)) {
        if ((abs(where_col-from_col) == 1) && (abs(where_row-from_row) == 1)) {
            board[where_col][where_row] = board[from_col][from_row];
            board[from_col][from_row] = 0;
            // Przekazanie tury drugiemu graczowi
            if (*state == RED_TURN) *state = WHITE_TURN;
            else *state = RED_TURN;
            return TRUE;
        }
        // TODO: Dodać bicie else if
        else {
            printf("Incorrect move!\n");
            return FALSE;
        }
    }
    else {
        printf("It's not your pawn!\n");
        return FALSE;
    }
    return FALSE;
}

int main() {
    enum FIELD_STATE board[8][9] = {FREE}; // Tablilca przechowująca planszę do gry, rząd 0 jest pusta, żeby móc używać oznaczeń z normalnej planszy
    enum GAME_STATE state = NEW_GAME; // Aktualny stan gry
    new_game(board, &state);
    print_board(board, state);
    move(board, &state, B, 6, C, 5);
    print_board(board, state);
    move(board, &state, A, 3, B, 4);
    print_board(board, state);
    return 0;
}
