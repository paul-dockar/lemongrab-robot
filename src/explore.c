#include "explore.h"

//declare local functions
void initialisePointersNULL(char *array[], char size);

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

void writeGlobalMap(char value, char x, char y) {
    global_map [x][y] = value;
}

void writeLocalMap(int adc_distance, char x, char y) {
    local_map [x][y] = adc_distance;
}

char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    char *closed_set [16];
    char *open_set [4];
    char *current_open_set;
    
    char g = 0;
    char h = 0;
    char f = g + h
    
    initialisePointersNULL(*closed_set[], 16);
    initialisePointersNULL(*open_set[], 4);

    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);
    
    global_map[robot_x, robot_y] = f;
    open_set[0] = &global_map[robot_x, robot_y];
    
    while (*open_set [0] != NULL) {
        char smallest_open_set = 255;
        for (char i = 3; i >= 0; i--) {
            if (*open_set [i] != 0 && smallest_open_set > *open_set [i]) {
                smallest_open_set = *open_set [i];
                current_open_set = open_set [i];
            }
        }
        
        
        
    }
    
    
    
    return 0;
}

void initialisePointersNULL(char *array[], char size) {
    for (char i = 0; i < size; i++) {
        *array[i] = NULL;
    }
}
