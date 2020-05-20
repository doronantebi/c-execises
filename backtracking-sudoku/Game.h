#include "solver.h"

#ifndef SP___HW3_GAME_H
#define SP___HW3_GAME_H

/*
* This method performs a whole game. It initializes the  board,
* receives commands from the user and executes them.
* It returns -1 when we need to terminate.
*/
int playGame(int numFixed);

/*
 * This method sets the cell (y, x) to be z in the current board.
 * x and y are 1-based. x is the column and y is the row.
 * It returns 0 always.
 */
int set (int x, int y, int z, struct Board *board);

/*
 * This method executes hint command only if the board is not solved.
 * x and y are 1-based. x is the column and y is the row.
 */
void hint (int x, int y, struct Board *board);

/*
 * This method validates if there is a possible solution to the user's board.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int validate(struct Board *board);

/*
 * This method restarts the game. It returns -1 if the program needs to be terminated. Otherwise, it returns 0.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 */
int restart(struct Board **pBoard);

/*
 * This method exits the game. It frees the struct before doing so.
 * It returns 2.
 */
int exitGame(struct Board **pBoard);


#endif
