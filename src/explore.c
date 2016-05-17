#include "explore.h"

unsigned char *closed_set [16];
unsigned char *open_set [4];

//declare local functions
void initialisePointersNULL(unsigned char *array[], char size);
void removeFromOpenSet(unsigned char *item_to_remove);
unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_node);

void setupExplore(void) {
    setupGlobalMap();
    setupLocalMap();
}

void setupGlobalMap(void) {
    for (char x = 0; x != GLOBAL_X; x++) {
        for (char y = 0; y != GLOBAL_Y; y++) {
            global_map [x][y] = 0;
        }
    }
}
    
void setupLocalMap(void) {
    for (char x = 0; x != LOCAL_X; x++) {
        for (char y = 0; y != LOCAL_Y; y++) {
            local_map [x][y] = 0;
        }
    }
}
    
void writeGlobalMap(unsigned char value, char x, char y) {
    global_map [x][y] = value;
}

void writeLocalMap(unsigned int adc_distance, char x, char y) {
    local_map [x][y] = adc_distance;
}

char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    unsigned char *current_open_set = 0;
    
    char g = 0;
    char h = 0;
    char f = g + h;
    
    //setup robot/goal position, clear open and closed sets
    initialisePointersNULL(closed_set, 16);
    initialisePointersNULL(open_set, 4);

    //writeGlobalMap(ROBOT, robot_x, robot_y);
    //writeGlobalMap(GOAL, goal_x, goal_y);

    writeGlobalMap(f, robot_x, robot_y);                //put starting node (robot) to 0.
    open_set[0] = &global_map[robot_x][robot_y];        //put starting node on open set list

    while (*open_set != 0) {
        unsigned char *successor_up;
        unsigned char *successor_right;
        unsigned char *successor_down;
        unsigned char *successor_left;

        unsigned char smallest_open_set = MAX;

        //find node with least f on the open list
        for (char i = 3; i >= 0; i--) {
            if (open_set [i] != 0 && smallest_open_set > *open_set [i]) {
                smallest_open_set = *open_set [i];
                current_open_set = open_set [i];
            }
        }
        
        //remove the chosen one from the open list
        removeFromOpenSet(current_open_set);
        
        //generate 4 successors from current node
        successor_up    = getNeighbourNodes(current_open_set, UP);
        successor_right = getNeighbourNodes(current_open_set, RIGHT);
        successor_down  = getNeighbourNodes(current_open_set, DOWN);
        successor_left  = getNeighbourNodes(current_open_set, LEFT);
    }
    
    
    
    return 0;
}

void initialisePointersNULL(unsigned char *array[], char size) {
    for (char i = 0; i < size; i++) {
        array[i] = 0;
    }
}

void removeFromOpenSet(unsigned char *item_to_remove) {
    for (char i = 3; i >= 0; i--) {
        if (open_set [i] == item_to_remove) {
            open_set [i] = 0;
        }
    }
}

unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_direction) {
    for (char x = 0; x != GLOBAL_X; x++) {
        for (char y = 0; y != GLOBAL_Y; y++) {
            if (current_node == &global_map[x][y]) {
                if (x == 0 && neighbour_direction == UP)    return 0;
                if (x == 4 && neighbour_direction == DOWN)  return 0;
                if (y == 0 && neighbour_direction == LEFT)  return 0;
                if (y == 5 && neighbour_direction == RIGHT) return 0;

                switch (neighbour_direction) {
                    case 1: x--; return (&global_map[x][y]);
                    case 2: y++; return (&global_map[x][y]);
                    case 3: x++; return (&global_map[x][y]);
                    case 4: y--; return (&global_map[x][y]);
                }
            }
        }
    }

}
