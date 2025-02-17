#include <stdio.h>

#define SIZE 3

void printBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '\0') {
                printf(" . ");
            } else {
                printf(" %c ", board[i][j]);
            }
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("---+---+---\n");
    }
}

int checkWin(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '\0') return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '\0') return board[0][i];
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '\0') return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '\0') return board[0][2];
    return 0;
}

void jugarTATETIrandom(char board[SIZE][SIZE], char nextMove[3]) {
    static char position[3]; // Usamos static para que la variable persista después de que la función termine
    nextMove = NULL; // Inicializamos la variable nextMove en NULL
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '\0') {
                board[i][j] = 'O';
                nextMove[0] = 'O';
                nextMove[1] = (i + 1) + '0';
                nextMove[2] = (j + 1) + '0';
            }
        }
    }
    return nextMove;
}

void jugarTATETI(char board[SIZE][SIZE], char nextMove[3]) {
    static char position[3]; // Usamos static para que la variable persista después de que la función termine
    nextMove = NULL; // Inicializamos la variable nextMove en NULL
    
    return nextMove;
}

void 
/*
int main() {
    char board[SIZE][SIZE] = {{'\0', '\0', '\0'}, {'\0', '\0', '\0'}, {'\0', '\0', '\0'}};
    jugarTATETI(board);
    return 0;
}
*/