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

void writeGlobalMap(char value, char x, char y) {
    global_map [x][y] = value;
}

void writeLocalMap(int adc_distance, char x, char y) {
    local_map [x][y] = adc_distance;
}

char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    char *closed_set [16];
    char *open_set [4];
    char *current_pos;

    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);
    
    current_pos = global_map[robot_x, robot_y];
    open_set[0] = current_pos;
    
    while (*open_set [0] != 0) {
        char os = 0;
        char cs = 0;
        for (int i=3; i>=0; i--) {
            char smallest_open_set = 255;
            if (*open_set [i] != 0 && smallest_open_set > *open_set [i]) {
                smallest_open_set = *open_set [i];   
            }
            
        }
        closed_set[cs] = open_set [os];
    }
    
    
    
    return 0;
}