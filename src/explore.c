#include "explore.h"

unsigned char *closed_set [CLOSED_SET_SIZE];
unsigned char *open_set [OPEN_SET_SIZE];

struct NEIGHBOUR {
    unsigned char *up;
    unsigned char *right;
    unsigned char *down;
    unsigned char *left;
} neighbour;

//declare local functions
void initialisePointersNULL(unsigned char *array[], char size);
void removeFromOpenSet(unsigned char *item_to_remove);
void pushToClosedSet(unsigned char *current_open_set);
void rearrangeOpenSet(void);
unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_node);
unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *goal, unsigned char *robot, char goal_x, char goal_y);

void setupExplore(void) {
    initialiseGlobalMap();
    initialiseLocalMap();
}

void initialiseGlobalMap(void) {
    for (char x = 0; x < GLOBAL_X; x++) {
        for (char y = 0; y < GLOBAL_Y; y++) {
            global_map [x][y] = 0;
        }
    }
}

void initialiseLocalMap(void) {
    for (char x = 0; x < LOCAL_X; x++) {
        for (char y = 0; y < LOCAL_Y; y++) {
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
    unsigned char *goal_position = &global_map[goal_x][goal_y];
    unsigned char *robot_position = &global_map[robot_x][robot_y];
    unsigned char ignore = 0;
    
    //setup robot/goal position, clear open and closed sets, clear global map
    initialiseGlobalMap();
    initialisePointersNULL(closed_set, CLOSED_SET_SIZE);
    initialisePointersNULL(open_set, OPEN_SET_SIZE);

    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);

    open_set[0] = robot_position;                       //put starting node on open set list

    //this while loop test doesnt exactly work atm FYI. but it is exited once the goal is found via early return call
    while (*open_set [0] != 0) {
        unsigned char smallest_open_set = MAX;

        //find node with least f on the open list
        for (char i = 0; i < OPEN_SET_SIZE; i++) {
            if (open_set [i] != 0 && smallest_open_set > *open_set [i]) {
                smallest_open_set = *open_set [i];
                current_open_set = open_set [i];
            }
        }
        
        //remove the chosen one from the open list
        removeFromOpenSet(current_open_set);
        
        //generate 4 neighbours from current node
        neighbour.up    = getNeighbourNodes(current_open_set, UP);
        neighbour.right = getNeighbourNodes(current_open_set, RIGHT);
        neighbour.down  = getNeighbourNodes(current_open_set, DOWN);
        neighbour.left  = getNeighbourNodes(current_open_set, LEFT);
        
        //for each neighbour; write f value to global map
        *neighbour.up       = checkNeighbour(neighbour.up, goal_position, robot_position, goal_x, goal_y);
        *neighbour.right    = checkNeighbour(neighbour.right, goal_position, robot_position, goal_x, goal_y);
        *neighbour.down     = checkNeighbour(neighbour.down, goal_position, robot_position, goal_x, goal_y);
        *neighbour.left     = checkNeighbour(neighbour.left, goal_position, robot_position, goal_x, goal_y);
        
        //please note this is currently incorrect returns (its mostly correct, but not exactly lol)
        //need to change to set the direction on the first iteration,
        //if that path is followed through to the goal, then return the direction from the first iteration
        if (*neighbour.up       == GOAL) return UP;
        if (*neighbour.right    == GOAL) return RIGHT;
        if (*neighbour.down     == GOAL) return DOWN;
        if (*neighbour.left     == GOAL) return LEFT;

        //put current_open_set onto closed set
        pushToClosedSet(current_open_set);
    }
    return 0;
}

void initialisePointersNULL(unsigned char *array[], char size) {
    for (char i = 0; i < size; i++) {
        array[i] = 0;
    }
}

void removeFromOpenSet(unsigned char *item_to_remove) {
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set [i] == item_to_remove) {
            open_set [i] = 0;
        }
    }
}

void pushToClosedSet(unsigned char *current_open_set){
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (closed_set[i] == current_open_set) {
            break;
        } else if (closed_set[i] == 0) {
            closed_set[i] = current_open_set;
            break;
        }
    }
}

void pushToOpenSet(unsigned char *current_neighbour){
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set[i] == current_neighbour) {
            break;
        } else if (open_set[i] == 0) {
            open_set[i] = current_neighbour;
            break;
        }
    }
}

unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_direction) {
    for (char x = 0; x < GLOBAL_X; x++) {
        for (char y = 0; y < GLOBAL_Y; y++) {
            if (current_node == &global_map[x][y]) {
                if (x == 0              && neighbour_direction == UP)    return &ignore;
                if (x == (GLOBAL_X - 1) && neighbour_direction == DOWN)  return &ignore;
                if (y == 0              && neighbour_direction == LEFT)  return &ignore;
                if (y == (GLOBAL_Y - 1) && neighbour_direction == RIGHT) return &ignore;

                switch (neighbour_direction) {
                    case 1: x--; return (&global_map[x][y]);
                    case 2: y++; return (&global_map[x][y]);
                    case 3: x++; return (&global_map[x][y]);
                    case 4: y--; return (&global_map[x][y]);
                }
            }
        }
    }
    return &ignore;
}

unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *goal, unsigned char *robot, char goal_x, char goal_y) {
    char distance=0;
    char distance_x=0;
    char distance_y=0;
    char pos_x=0;
    char pos_y=0;

    if (neighbour == &ignore) return MAX;
    if (neighbour == goal) return GOAL;
    if (neighbour == robot) return ROBOT;
    
    for (char x = 0; x < GLOBAL_X; x++) {
        for (char y = 0; y < GLOBAL_Y; y++) {
            if (neighbour == &global_map[x][y]) {
                pos_x = x;
                pos_y = y;
                break;
            }
        }
    }
    
    //this is just using the h value for f = g + h. Might add in g later where g = 1 (since it is always 1 anyway no need to over complicate it.
    distance_x = pos_x - goal_x;
    distance_y = pos_y - goal_y;
    distance = abs(distance_x) + abs(distance_y);

    //if a node with same position as successor is in the OPEN list, skip it
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (neighbour == open_set[i]) {
            return distance;
        }
    }
    //if a node with same position as successor is in the CLOSE list, skip it
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (neighbour == closed_set[i]) {
            return distance;
        }
    }
    //otherwise add node to open list
    pushToOpenSet(neighbour);
    return distance;
}
