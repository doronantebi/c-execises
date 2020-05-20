#include <stdio.h>
#include "Game.h"
#include "MainAux.h"
#include <stdlib.h>

int main( int argc, char* argv[]) {
    int numFixed, check;
    int seed;
    if (argc < 2){
        seed = 5;
    }
    else {
        check = sscanf(argv[1], "%d", &seed);
        if (check == -1){ /* EOF was entered*/
            return 0;
        }
        if (check == 0){
          printFunctionError("sscanf");
          return 0; /* we need to exit */
        }
    }
    srand(seed);
    if (input(&numFixed) == -1){
        return 0; /* we need to exit */
    }

    return playGame(numFixed);
}
