#include "explore.h"

void setupExplore(void) {
    setupGlobalMap();
    setupLocalMap();
}

void setupGlobalMap(void) {
    char x = 0;
    char y = 0;
    for (x; x!=4; x++) {
        for (y; y!=5; y++) {
            global_map [x][y] = 0;
        }
    }
}

void setupLocalMap(void) {
    char x = 0;
    char y = 0;
    for (x; x!=3; x++) {
        for (y; y!=3; y++) {
            local_map [x][y] = 0;
        }
    }
}
