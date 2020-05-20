#include <stdlib.h>
#include "solver.h"
#define RAND 'R'
#define DET 'D'


int doNextBacktrack(struct Board *board, int i, int j, char state);
int recBacktracking(struct Board *board, int i, int j, char state);



/*REMEMBER TO CHECK CHECKBLOCK METHOD!!! */


/*
 * Checks if val is in row i.
 */
int checkRow(int val, int *mat, int n, int m, int i){
    int col, size = n * m ;
    for (col = 0; col < size ; col++) {
        if(mat[i * size + col] == val){
            return 1;
        }
    }
    return 0;
}

/*
 * Check if val is in column j.
 */
int checkColumn(int val, int *mat, int n, int m, int j){
    int row, size = n * m ;
    for (row = 0; row < size ; row++) {
        if(mat[row*size + j] == val){
            return 1;
        }
    }
    return 0;
}

/*
 * Check if val is in the block of (i,j).
 */
int checkBlock(int val, int *mat, int n, int m, int i, int j){
    int blockRowIndex = i/m ;  /*  ???????   */
    int blockColIndex = j/n ;  /*  ???????   */
    int row, col;
    for(row = blockRowIndex*m ; row < (blockRowIndex + 1)*m ; row++){
        for( col = blockColIndex*n ; col < (blockColIndex+1)*n ; col++) {
            if(mat[row*n*m + col] == val){
                return 1;
            }
        }
    }
    return 0;
}

/*
 * This method checks if val is in row, column or block.
 */
int doNeighboursHave(int val, int *mat, int n, int m, int i, int j){
    if(checkRow(val, mat, n, m, i)){
        return 1;
    }
    if(checkColumn(val, mat, n, m, j)){
        return 1;
    }
    if(checkBlock(val, mat, n, m, i, j)){
        return 1;
    }
    return 0;
}

/*
 * The method checks if the (i, j) cell in the sudoku grid is a fixed cell.
 */
int isFixed(int i, int j, struct Board *board){
    int k;
    if(board->fixedIndices == NULL){ /* we don't have fixed indices */
        return 0;
    }
    for (k = 0 ; k < 2*board->Fixed ; k += 2){
        if ((board->fixedIndices[k] == i) && (board->fixedIndices[k+1]) == j){
            return 1; /* (i,j) is a fixed index */
        }
    }
    return 0;
}

/*
 * This method goes through all numbers 1,...,n*m
 * and returns an array of size n*m such that
 * for any index 0 <= k <= n*m - 1:
 * values[k] = 1   <- i.f.f ->  some neighbor of the (i, j) cell contains k + 1
 * otherwise, values[k] = 0.
 */
int* findLegalInput(struct Board *board, int i, int j){
    int *values, n = board->n, m = board->m ;
    int size = n*m, val;
    values = malloc(size*sizeof(int));
    if (values == NULL){
        board->status = MALLOC_ERROR;
        return NULL;
    }
    for (val = 0 ; val < size ; val ++){
        values[val] = doNeighboursHave(val+1, board->solutionBoard, n, m, i, j);
    }
    return values;
}

/*
 * This method checks how many times toCount appears in array.
*/
int countNum(int* array, int length, int toCount){
    int i;
    int count = 0;
    for(i=0 ; i<length ; i++){
        if(array[i] == toCount){
            count ++;
        }
    }
    return count;
}

/*
 * This method counts the amount of zeros in array.
 */
int countZeros(int* array, int length){
    return countNum(array, length, 0);
}

/*
 * This method receives an array with 1's and 0's,
 * and the size of this array;
 * in addition, it receives an additional array: arrayToFill in size: arraySize.
 * It should suffice : arraySize == #zeros in arrayOfValues.
 * For every index 0<=i<size such that arrayOfValues[i] == 0, the method will place
 * in arrayToFill the number i+1. It is done in increasing order.
 * The method returns 1 if there are exactly arraySize relevant indices,
 * otherwise, returns 0.
 *
 */
int fillLegalValues(int* arrayOfValues, int size, int* arrayToFill, int arraySize){
    int k , j = 0;
    for (k = 0 ; k < size; k++) {
        if(arrayOfValues[k] == 0){
            arrayToFill[j] = k + 1 ;
            j++;
        }
    }
    if( j == arraySize ){
        return 1;
    }
    return 0; /* should never get here */
}


/*
 * This method returns the next value.
 */
int getNextVal(int* legalValuesArray, int length, char state){
    if (state == DET){
        return legalValuesArray[0];
    }
    if (state == RAND){
        return legalValuesArray[rand() % length];
    }
    return -1; /* should never get here */
}

/*
 * The method receives a pointer to array to change it outside the method.
 * The method moves every integer right to toRemove one place left and reallocates
 * the given array so that its size is one fewer than the size of the received array.
 */
int *removeFromArray(int **pArray, int arraySize, int toRemove){
    int i, j;
    int *array = *pArray;
    for (i = 0 ; i < arraySize ; i++){
        if (array[i] == toRemove){
            break;
        }
    }
    for (j = i ; j < arraySize - 1; j ++){
        array[j] = array[j + 1];
    }
    return realloc(array, (arraySize-1)* sizeof(int));
}

/*
 * This method randomizes Fixed amount of fixed cells,
 * and returns their places.
 */
int* createFixedIndices(int n, int m, int Fixed){
    int* fixedIndices;
    int* checkArray;
    int row, col, i ;
    int size = n*m;
    checkArray = calloc(size*size, sizeof(int)); /* array is filled with zeros*/
    if (checkArray == NULL){
        return NULL;
    }
    fixedIndices = calloc(2 * Fixed, sizeof(int)); /* array of consecutive pairs that
                                                    * represent a place in the sudoku grid*/
    if (fixedIndices == NULL){
        return NULL;
    }
    i = 0;
    while ( i < 2*Fixed ){
        col = rand() % size;
        row = rand() % size;
        if(checkArray[row*size+col] == 0){
            checkArray[row*size+col] = 1;
            fixedIndices[i]= row;
            fixedIndices[i + 1] = col;
            i += 2;
        }
    }
    free(checkArray);
    return fixedIndices ;
}

/*
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j){
    return j == size - 1;
}

/*
 * This method checks if the current cell is last in column, assuming length of column is size.
 */
int isLastInCol(int size, int i){
    return i == size - 1;
}

/*
 * This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
 */
int isLastCellInMatrix(int size, int i, int j){
    return isLastInCol(size, i) && isLastInRow(size, j);
}

/*
 * This method performs the next step of backtracking.
 */
int doNextBacktrack(struct Board *board, int i, int j, char state) {
    int n = board->n;
    int m = board->m;

    if (isLastCellInMatrix(n * m, i, j)) { /* matrix is full and all cells are legal */
        board->status = SOLVED;
        return 1;
    }
    else{
        if (isLastInRow(n * m, j)) { /* This is the last object in the row, now we need to begin the next row */
            return recBacktracking(board, i + 1, 0, state);
        }
        else { /* This is not the last object in row -> we will go to the next object in row  */
            return recBacktracking(board, i, j + 1, state);
        }
    }
}

/*
 * This method performs backtracking. state determines if it is deterministic or random.
 */
int recBacktracking(struct Board *board, int i, int j, char state) {
    int nextVal, numLegalValues;
    int res, *allValues, *onlyLegalValues, *toBeFreed;
    int *sudokuGrid = board->solutionBoard; /* This is the board we change */
    int *usersBoard = board->usersBoard;
    int n = board->n;
    int m = board->m;

    if (isFixed(i, j, board) || (usersBoard[n*m*i + j] != 0)){ /* we don't want to change cells
                                                                * that were filled by the user */
        return doNextBacktrack(board, i, j, state);
    }

    allValues = findLegalInput(board, i, j);  /* returns a binary array that specifies which value is
                                                            * legal to the (i, j) cell in sudokuGrid */
    if (allValues == NULL){
        return 0;
    }

    numLegalValues = countZeros(allValues, n * m);     /* counts how  many legal values there
                                                                * are for the (i, j) cell*/

    if (numLegalValues == 0){
        free(allValues);
        return 0;
    }

    onlyLegalValues = malloc(numLegalValues * sizeof(int)); /* array of legal values */

    if (onlyLegalValues == NULL){
        free(allValues);
        board->status = MALLOC_ERROR;
        return 0;
    }

    fillLegalValues(allValues, n * m, onlyLegalValues, numLegalValues);  /* fills onlyLegalValues with
                                                                        * legal values only*/

    free(allValues);

    while (numLegalValues > 1){
        nextVal = getNextVal(onlyLegalValues, numLegalValues, state);
        sudokuGrid[i * (m * n) + j] = nextVal;
        res = doNextBacktrack(board, i, j, state);
        if (res == 1){
            free(onlyLegalValues);
            return 1;
        }
        sudokuGrid[i * (m * n) + j] = 0;
        toBeFreed = removeFromArray(&onlyLegalValues, numLegalValues, nextVal);
        if (toBeFreed == NULL){ /* realloc failed */
            free(onlyLegalValues);
            board->status = REALLOC_ERROR;
            return 0;
        }
        onlyLegalValues = toBeFreed;
        numLegalValues--;
    }

    if (numLegalValues == 1){
        nextVal = onlyLegalValues[0];
        sudokuGrid[i * (m * n) + j] = nextVal;
        free(onlyLegalValues);
        res = doNextBacktrack(board, i, j, state);
        if (res == 1){
            return 1;
        }
        sudokuGrid[i * (m * n) + j] = 0;
    }

    return 0;

}


/*
 * This method copies a currentGrid to retGrid.
 */
void duplicateBoard(int* currentGrid, int *retGrid, int size){
    int i,j;
    for (i = 0; i < size ; i++) { /* for each row */
        for (j = 0; j < size ; j++) { /* for each column */
            retGrid[i*size+j] = currentGrid[i*size+j];
        }
    }
}

/*
 * This method creates and returns an empty sudoku grid.
 */
int* createEmptyBoard(int n, int m){
    int *emptyBoard;
    emptyBoard = calloc(n*m*n*m, sizeof(int));
    return emptyBoard;
}

int backtracking(struct Board *board, char state){
    int res;
    board->status = CAN_BE_SOLVED;
    res = recBacktracking(board, 0, 0, state);
    if (board->status == CAN_BE_SOLVED){
        board->status = CANNOT_BE_SOLVED;
    }
    return res;
}

/*
 * This method solves the current user's board by copying it to the solution board and
 * solving it using deterministic backtracking.
 * It returns 1 if there is a solution. Otherwise, it returns 0.
 * If the board is unsolvable the solution board remains the same as before (does not change).
 */
int solve (struct Board *board){
    int n = board->n, m = board->m, res ;
    int *newBoard = calloc(n*m*n*m, sizeof(int));
    duplicateBoard(board->solutionBoard, newBoard, n*m);
    duplicateBoard(board->usersBoard, board->solutionBoard, n*m ); /* makes a copy of the grid */
    res = backtracking(board, DET);
    if (res == 0){
        if (board->status == CANNOT_BE_SOLVED){
            duplicateBoard(newBoard, board->solutionBoard, n*m);
        }
    }
    free(newBoard);
    return res;
}

/*
 *  This method creates a solvable board by creating a solution board using random backtracking.
 *  It raffles random fixed cells and then copies the fixed cells in solution board to the user's board.
 * A cell with no fixed number will contain 0.
 * It returns a pointer to the new board we have created.
 */
struct Board *generatePuzzle (int n, int m, int Fixed){
    int *fixedIndices;
    struct Board *board = malloc(sizeof(struct Board));
    int i;
    if (board == NULL){
        return NULL;
    }
    board->n = n;
    board->m = m;
    board->Fixed = Fixed;
    board->status = CAN_BE_SOLVED;
    board->usersBoard = createEmptyBoard(n, m);
    board->solutionBoard = createEmptyBoard(n, m);
    if (board->solutionBoard == NULL || board->usersBoard == NULL){ /* we have got a calloc-error*/
        board->status = CALLOC_ERROR;
        return board; /* terminates */
    }
    board->fixedIndices = NULL;
    if (backtracking(board, RAND) == 0){ /* we couldn't create a valid board. Should never suffice that,
                                            * unless we have had calloc/malloc error */
        return board; /* terminates */
    }

    fixedIndices = createFixedIndices(n, m, Fixed); /* randomizes #Fixed amount of fixed cells */

    if (fixedIndices == NULL){ /* we have got a calloc-error*/
        board->status = CALLOC_ERROR;
        return board; /* terminates */
    }

    board->fixedIndices = fixedIndices;
    for (i = 0; i < 2*Fixed; i += 2){  /* we put all the values we have got in the solution board
                                        * that are in the fixed indices in usersBoard */
        board->usersBoard[fixedIndices[i]*n*m + fixedIndices[i+1]] =
                board->solutionBoard[fixedIndices[i]*n*m + fixedIndices[i+1]];
    }
    board->countFilledCells = Fixed;

    return board;
}
