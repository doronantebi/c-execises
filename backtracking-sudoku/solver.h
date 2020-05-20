
#ifndef SP_HW3_SOLVER_H
#define SP_HW3_SOLVER_H

enum Status { SOLVED = 0 , CAN_BE_SOLVED = 1 , CANNOT_BE_SOLVED = 2,
    MALLOC_ERROR = 3, CALLOC_ERROR = 4, REALLOC_ERROR = 5};

struct Board {
    int *solutionBoard;  /* This field is the one which updates,
                          * and eventually will contain a board with
                          * a valid solution. */
    int n;
    int m;
    int *usersBoard;     /* This module will initialize this board. */
    int *fixedIndices;   /* This array is created during generation,
                          * and won't change. */
    int Fixed;           /* The size of fixed Indices (divided by two). */
    enum Status status;  /* 0 = SOLVED, 1 = CAN BE SOLVED,
                          * 2 = CANNOT BE SOLVED, 3 = MALLOC ERROR,
                          * 4 = CALLOC ERROR, 5 = REALLOC ERROR */
    int countFilledCells /* Holds the amount of filled cells.*/;
};

/*
 * This method solves the current user's board by copying it to the solution board and
 * solving it using deterministic backtracking.
 * It returns 1 if there is a solution. Otherwise, it returns 0.
 * If the board is unsolvable the solution board remains the same as before (does not change).
 */
int solve (struct Board *board);

/*
 * This method creates a legal sudoku puzzle:
 * with n blocks in each column and m blocks in each row,
 * each block has m rows and n columns.
 * total : grid size is n*m x n*m.
 * the returned puzzle will have fixed amount of fixed cells.
 */
struct Board *generatePuzzle (int n, int m , int Fixed);

/*
 * This method checks if a cell is a fixed cell.
 */
int isFixed(int i, int j, struct Board *board);

/*
 * This method returns 1 if some neighbour of (i,j) in mat contains val,
 * otherwise, returns 0.
 */
int doNeighboursHave(int val, int *mat, int n, int m, int i, int j);

#endif

