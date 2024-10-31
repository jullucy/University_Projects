/**
 * "Reversi"
 *
 * Project for the Introduction to Programming course.
 * Compilation command:
 * gcc @options reversi.c -o reversi
 * This program allows you to play the game "Reversi."
 * It displays a prompt indicating which player's turn it is and shows
 * the available legal moves.
 * It reads the current player's command.
 * If it reads the command "=" to stop the game, it terminates.
 * If it reads the command "-" to skip a turn, it returns to the start of the loop.
 * If it reads a move command (field name), it performs that move
 * and returns to the start of the loop.
 * Author: Julia Lucyn
 */

#include <stdio.h>

/* Board dimensions */

/**
 * Number of rows on the board.
 */
#define ROWS 8

/**
 * Number of columns on the board.
 */
#define COLUMNS 8

/* End of game condition */

/**
 * Ending the game ("=") or skipping a round ("-").
 */
#define END 100

/* Board setup */

/**
 * Marking empty squares on the board.
 */
#define EMPTY 0

/**
 * Marking board squares where black pieces are located.
 */
#define BLACK 1

/**
 * Marking board squares where white pieces are located.
 */
#define WHITE 2

/* Board preparation */

/**
 * Places four initial pieces on the 8x8 array 'b' so that:
 * Black pieces are on d5 and e4
 * White pieces are on d4 and e5.
 */
void setupBoard(int board[][COLUMNS]) {
    int i, j;

    for (i = 0; i < ROWS; ++i) {
        for (j = 0; j < COLUMNS; ++j) {
        board[i][j] = EMPTY;
        }
    }

    board[ROWS / 2 - 1][COLUMNS / 2 - 1] = WHITE;
    board[ROWS / 2][COLUMNS / 2 - 1] = BLACK;
    board[ROWS / 2 - 1][COLUMNS / 2] = BLACK;
    board[ROWS / 2][COLUMNS / 2] = WHITE;
}

/* Direction determination */

/**
 * Checks if given coordinates are within the array. 'row' specifies the row index, and
 * 'col' specifies the column index.
 */
bool isWithinArray(int row, int col) {
    return ((row >= 0) && (row < ROWS) && (col >= 0) && (col < COLUMNS));
}

/**
 * Checks if there are opponent's pieces in a given direction.
 * Returns true if there are; false otherwise.
 * 'row' specifies the row index of the square around which we are checking.
 * 'col' specifies the column index of the square around which we are checking.
 * 'colour' specifies the color of the current player.
 * 'dR' is the row direction offset (integer range [-1,1]).
 * 'dC' is the column direction offset (integer range [-1,1]).
 */
bool checkDirection(int b[][COLUMNS], int row, int col, int colour, int dR, int dC) {
    int i = 1;
    int oppColor;
    int signal = 0;

    if (colour == WHITE) {
        oppColor = BLACK;
    } else {
        oppColor = WHITE;
    }

    while ((isWithinArray(row + dR * i, col + dC * i))
        && (b[row + dR * i][col + dC * i] != colour)
        && (b[row + dR * i][col + dC * i] != EMPTY && signal == 0)) {
        i++;

        if ((isWithinArray(row + dR * i, col + dC * i))
            && ((b[row + dR * i][col + dC * i] == colour))) {
            signal = 1;
        } else if ((isWithinArray(row + dR * i, col + dC * i))
            && (b[row + dR * i][col + dC * i] == oppColor)) {
            signal = 0;
        } else if (isWithinArray(row + dR * i, col + dC * i)) {
            if (b[row + dR * i][col + dC * i] == colour) {
                signal = 1;
            } else {
                signal = 2;
            }
        } else {
            signal = 2;
        }
    }

    return (signal == 1);
}

/* Displaying the player color */

/**
 * Displays the current player's color as 'C' for black or 'B' for white.
 */
void displayColor(int colour) {
    if (colour == 1) {
        printf("%s", "C");
    } else {
        printf("%s", "B");
    }
}

/**
 * Searches for possible moves for the current player and displays them (if any).
 *
 * 'colour' specifies the color of the current player.
 *
 * 'b' is the board (8x8 array) with the current arrangement of pieces.
 */
void displayMoves(int b[][COLUMNS], int colour) {
    int row, col, dR, dC;
    bool signal;

    for (row = 0; row < ROWS; ++row) {
        for (col = 0; col < COLUMNS; ++col) {
            if (b[row][col] == EMPTY) {
                signal = false;

                for (dR = -1; dR <= 1 && !signal; ++dR) {
                    for (dC = -1; dC <= 1 && !signal; ++dC) {
                        if ((dR != 0 || dC != 0) && checkDirection(b, row, col, colour, dR, dC)) {
                            printf(" %c%d", row + 'a', col + 1);
                            signal = true;
                        }
                    }
                }
            }
        }
    }
}

/* Reading and changing the board state */

/**
 * Converts a letter 'n' to the corresponding numerical position in the alphabet - 1.
 */
int letterToNumber(char n) {
    return  n - 'a';
}

/**
 * Reads coordinates entered by the player and stores them in the array 'helper'.
 *
 * If the player has opted to skip the turn, it sets row to END.
 *
 * If the end of game command is entered, it sets row and col to END.
 */
void readMove(char helper[], int *row, int *col) {
    int c = getchar();
    int w = 0;
    while (helper[w] != 0) {
        helper[w] = 0;
        ++w;
    }
    w = 0;
    while ((c != '\n') && (c != EOF)) {
        helper[w] = (char) c;
        ++w;
        c = getchar();
    }
    w = 0;
    if (helper[w] == '=') {
        *col = END;
        *row = END;
    } else if (helper[w] != '-') {
        *row = letterToNumber(helper[w]);
        ++w;
        *col = helper[w] - '0';
    } else {
        *row = END;
    }
}

/* Making a move specified by the player */

/**
 * Changes the color of all pieces in the specified direction until it reaches
 * a piece of the current player's color.
 *
 * 'colour' specifies the color of the current player.
 *
 * The direction is defined by vector [dR, dC].
 *
 * 'row' and 'col' are the indexes of the field chosen by the player.
 */
void flipPieces(int board[][COLUMNS], int row, int col, int colour, int dR, int dC) {
    int i = 1;

    while (board[row + dR * i][col + dC * i] != colour) {
        board[row + dR * i][col + dC * i] = colour;
        ++i;
        }
}

/**
 * Searches around the specified field for pieces whose color should be flipped
 * and changes them. Changes the color of the specified field to the player's color.
 *
 * 'row' is the row index of the field to be checked.
 *
 * 'col' is the column index of the field to be checked.
 *
 * 'colour' is the color of the current player.
 */
void makeMove(int board[][COLUMNS], int colour, int row, int col) {
    int dR, dC;

    if ((isWithinArray(row, col)) && (board[row][col] == EMPTY)) {
        for (dR = -1; dR <= 1; dR++) {
            for (dC = -1; dC <= 1; dC++) {
                if ((dR != 0 || dC != 0) && (checkDirection(board, row, col, colour, dR, dC))) {
                board[row][col] = colour;
                flipPieces(board, row, col, colour, dR, dC);
                }
            }
        }
    }
}

/**
 * Runs the game on board 'b'.
 *
 * 'helper' is a helper array that records moves entered by the player.
 */
void playGame(int board[][COLUMNS], char helper[]) {
    setupBoard(board);
    int col = 0;
    int row = 0;
    int colour = WHITE;

    while (row != END || col != END) {
        colour = 3 - colour;
        displayColor(colour);
        displayMoves(board, colour);
        printf("\n");
        readMove(helper, &row, &col);

        if (row != END) {
            col--;
            makeMove(board, colour, row, col);
        }
    }
}

/**
 * Starts the game.
 */
int main(void) {
    char helper[] = {0, 0, 0};
    int board[ROWS][COLUMNS];
    playGame(board, helper);
    return 0;
}
