
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "solver.h"
#include "Parser.h"
#include "MainAux.h"
#define LENGTH 1024


/*
 * This method returns if the current board is full.
 */
int isFull(struct Board *board){
    return board->countFilledCells == board->n * board-> m * board->n * board-> m;
}

/*
 * This method prints a line of 34 "-".
 */
void printLine(){
    int i;
    for (i = 0; i < 34; i++) {
        printf("-");
    }
    printf("\n");
}

/*
 * This method prints the board in the desired format.
 */
void printBoard(int *sudokuGrid, struct Board *board){
    int colBlock, rowBlock, row, col, cell;
    for (rowBlock = 0; rowBlock <3 ; rowBlock++) {
        printLine();
        for (row = 0; row < 3; row++) {
            printf("| ");
            for (colBlock = 0; colBlock < 3; colBlock++) {
                for (col = 0; col < 3; col++) {
                    cell = sudokuGrid[(row+rowBlock*3) * 9 + colBlock * 3 + col];
                    if (cell != 0) {
                        if (isFixed(row+rowBlock*3, colBlock * 3 + col, board)) {
                            printf(".%d ", cell);
                        } else {
                            printf(" %d ", cell);
                        }
                    } else {
                        printf("   ");
                    }
                }
                printf("|");
                if (colBlock < 2){
                    printf(" ");
                }
            }
            printf("\n");
        }
    }
    printLine();
}

/*
 * This method checks if there are any standard failures.
 * If there are, it returns -1. Otherwise, it returns 0.
 */
int checkErrors(struct Board *board){
    if (board == NULL){  /* allocation for struct failed */
        printFunctionError("malloc");
        return -1;
    }

    if (board->status == CALLOC_ERROR){
        printFunctionError("calloc");
        return -1;
    }

    if (board->status == MALLOC_ERROR){
        printFunctionError("malloc");
        return -1;
    }

    if (board->status == REALLOC_ERROR){
        printFunctionError("realloc");
        return -1;
    }
    return 0;
}

/*
 * This method initializes the board by calling to generatePuzzle.
 * It returns -1 if the program needs to be terminated. Otherwise, it returns 0.
 */
int initializeBoard(int numFixed, struct Board **board) {
    int terminate;

    *board = generatePuzzle(3, 3, numFixed);

    terminate = checkErrors(*board);

    if (terminate == 0){
        printBoard((*board)->usersBoard, *board);
    }
    return terminate;
}

/*
 * This method sets the cell (y, x) to be z in the current board.
 * x and y are 1-based. x is the column and y is the row.
 * It returns 0 always.
 */
int set (int x, int y, int z, struct Board *board){
    int n, m;
    if (isFull(board)){
        printInvalidCommand();
        return 0;
    }
    x--;
    y--;
    n = board->n;
    m = board->m;
    if (isFixed(y, x, board)){
        printf("Error: cell is fixed\n");
        return 0;
    }

    if (z == 0){
        (board->usersBoard)[y*n*m + x] = 0;
        board->countFilledCells--;
        printBoard(board->usersBoard, board);
        return 0;
    }

    if (doNeighboursHave(z, board->usersBoard, n, m, y, x)){
        printf("Error: value is invalid\n");
        return 0;
    }

    (board->usersBoard)[y*n*m + x] = z;
    board->countFilledCells++;
    printBoard(board->usersBoard, board);
    if (isFull(board)){
        printf("Puzzle solved successfully\n");
    }
    return 0;
}

/*
 * This method executes hint command only if the board is not solved.
 * x and y are 1-based. x is the column and y is the row.
 */
void hint (int x, int y, struct Board *board){
    if (!isFull(board)){
        x--;
        y--;
        printf("Hint: set cell to %d\n", (board->solutionBoard)[(board->n)*(board->m)*y + x]);
    }
    else{
        printInvalidCommand();
    }
}

/*
 * This method validates if there is a possible solution to the user's board.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int validate(struct Board *board){
    int isOK;
    if (isFull(board)){
        printInvalidCommand();
        return 0;
    }

    isOK = solve(board);
    if (isOK == 1){
        printf("Validation passed: board is solvable\n");
        return 0;
    }
    /* isOK == 0 */
    printf("Validation failed: board is unsolvable\n");
    return checkErrors(board);
}

/*
 * This method frees all memory allocated for the struct board.
 */
void freeStruct(struct Board *board){
    free(board->solutionBoard);
    free(board->fixedIndices);
    free(board->usersBoard);
    free(board);
}

/*
 * This method restarts the game. It returns -1 if the program needs to be terminated. Otherwise, it returns 0.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int restart(struct Board **board){
    int numFixed;
    if (input(&numFixed) == -1){
        return -1;
    }
    freeStruct(*board);
    return initializeBoard(numFixed, board);
}

/*
 * This method exits the game. It frees the struct before doing so.
 * It returns 2.
 */
int exitGame(struct Board **board){
    printf("Exiting...\n");
    freeStruct(*board);
    return 2;
}

/*
 * This method performs a whole game. It initializes the  board,
 * receives commands from the user and executes them.
 * It returns -1 when we need to terminate.
 */
int playGame(int numFixed){
    char command[LENGTH];  /*Last cell is '\0' thus we need to allocate array of length LENGTH+1*/
    int res;
    struct Board *board;

    if (initializeBoard(numFixed, &board) == -1){  /* we should terminate*/
        return -1;
    }

    while (fgets(command, LENGTH, stdin) != NULL){  /* We have not reached EOF*/
        if (command[0] == '\n'){ /* Line is empty. */
            continue;
        }
        /*The line is not blank*/
        res = interpret(command, &board);
        if (res == -1){
            return -1;
        }
        if (res == 2) { /* exit command was entered*/
            return 0;
        }
    }
    /* We have reached EOF, there are no more commands to execute*/
    return exitGame(&board);
}
