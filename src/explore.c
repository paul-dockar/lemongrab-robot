#include "explore.h"

void explore(void) {
    char map [4][5];        //4x5 array for the map of the maze. 4 rows. 5 columns.
    
    char x = 0;
    char y = 0;
    for (x; x!=4; x++) {
        for (y; y!=5; y++) {
            map [x][y] = 0;
        }
    }
}
