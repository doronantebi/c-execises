
#include "Parser.h"
#include <stdio.h>
#include <string.h>
#include "Game.h"

/*
 * The function prints a message to the user that the command is invalid.
 */
void printInvalidCommand(){
    printf("Error: invalid command\n");
}

/*
 * This method assumes the command entered is set, checks the validity of the rest of the command and executes it.
 * It returns 0.
 */
int interpretSet(char *token, struct Board *board){
    int i = 0;
    int arr[3];
    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 3){
        sscanf(token, "%d", &arr[i]);  /* We assume only integers are entered and
                                                    that they are valid and correct.*/
        i++;
    }
    if (i < 3){
        printInvalidCommand();
    }
    else{
        return set(arr[0], arr[1], arr[2], board);
    }
    return 0;

}

/*
 * This method assumes the command entered is hint, checks the validity of the rest of the command and executes it.
 */
void interpretHint(char *token, struct Board *board){
    int i = 0;
    int arr[2];

    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 2){
        sscanf(token, "%d", &arr[i]);  /* We assume only integers are entered and
                                                    that they are valid and correct.*/
        i++;
    }
    if (i < 2){
        printInvalidCommand();
    }
    else{
        hint(arr[0], arr[1], board);
    }

}

/*
 * This method interprets the command the user entered and
 * executes it by calling to the proper method in Game.
 * It returns -1 if we need to terminate. Otherwise, it returns 0.
 * If exit command has been received, it returns 2.
 */
int interpret(char *command, struct Board **pBoard){
    char *token;
    struct Board *board = *pBoard;
    token = strtok(command, " \t\r\n");
    if (token != NULL) { /* First token exists*/
        if (strcmp(token, "set") == 0){
            return interpretSet(token, board);
        }
        else{
            if (strcmp(token, "hint") == 0){
                interpretHint(token, board);
                return 0;
            }
            else{
                if (strcmp(token, "validate") == 0){
                    return validate(board);
                }
                else{
                    if (strcmp(token, "restart") == 0){
                        return restart(pBoard);
                    }
                    else{
                        if (strcmp(token, "exit") == 0){
                            return exitGame(pBoard);
                        }
                        else{
                            printInvalidCommand();
                        }
                    }
                }
            }
        }
    }

    return 0;
}


