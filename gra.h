#include <stdio.h>
#include <string.h>

enum COL {A, B, C, D, E, F, G, H};
enum GAME_STATE {NEW_GAME, WHITE_TURN, RED_TURN, WHITE_WIN, RED_WIN};
enum FIELD_STATE {FREE, WHITE_PAWN, WHITE_KING, RED_PAWN, RED_KING}; // Możliwe stany dla każdego z pól
enum MOVE_ERROR {NO_ERROR, ERROR_WRONG_STATE, ERROR_FIELD_INVALID, ERROR_FIELD_TAKEN, ERROR_WRONG_FIGURE, ERROR_FORCE_CAPTURE, ERROR_NO_ENEMY, ERROR_MOVE_INCORRECT};
enum BOOL {FALSE, TRUE}; // W C nie ma boola, więc go dodaję
enum COLOR {WHITE=-1, NO_COLOR, RED}; // Kolor, pomaga w ustaleniu dozwolonego kierunku ruchu pionka
enum TYPE {PAWN, KING}; // Typ figury

// Potrzebne funkcje
void new_game(enum FIELD_STATE board[8][9], enum GAME_STATE *state); // Rozpoczyna nową grę
void generate_board(enum FIELD_STATE board[8][9], enum GAME_STATE state, char *board_string);  // Generuje podgląd aktualnego rozkładu pionków
void generate_error_message(enum MOVE_ERROR move_error_no, char* message); // Generuj wiadomość o błędzie
enum MOVE_ERROR move(enum FIELD_STATE board[8][9], enum GAME_STATE *state, int last_used_figure[3], int from_col, int from_row, int where_col, int where_row); // Poruszanie figurą
enum COLOR is_win(enum FIELD_STATE board[8][9], enum GAME_STATE *state); // Czy nastąpiło zwycięstwo, zwraca kolor zwycięzcy lub NO_COLOR
