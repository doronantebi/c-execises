
#include <stdio.h>


/*
 * This method prints a message to the user
 */
void printFunctionError(char *str){
    printf("Error: %s has failed\n", str);
}

/*
 * The function checks if numFixed is a number between 0 and 80.
 * If it is, it returns 1. Otherwise it returns 0.
 */
int checkInput(int numFixed){
    if (numFixed >= 0 && numFixed <= 80){
        return 1;
    }
    return 0;
}

/*
 * This method asks for input from the user and checks if it is EOF
 * or if no integer was received. If so, it returns -1. Otherwise, it returns 0.
 */
int askForInput(int *pNumFixed){
    int numEntered;
    printf("Please enter the number of cells to fill [0-80]:\n");
    numEntered = scanf("%d", pNumFixed);
    if (numEntered == -1){  /*we received EOF*/
        printf("Exiting...\n");
        return -1;
    }
    if (numEntered == 0){
        printf("Error: scanf has failed\n");
        return -1;
    }
    return 0;
}

/*
 * This method receives input from the user until legal value is entered. If askForInput returns -1,
 * so does this method. Otherwise, it returns 0.
 */
int input(int *pNumFixed){
    if (askForInput(pNumFixed) == -1){
        return -1;
    }
    while (!checkInput(*pNumFixed)){
        printf("Error: invalid number of cells to fill (should be between 0 and 80)\n");
        if (askForInput(pNumFixed) == -1){
            return -1;
        }
    }
    return 0;
}
