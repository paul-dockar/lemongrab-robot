#include "explore.h"

unsigned char *closed_set [CLOSED_SET_SIZE];
unsigned char *open_set [OPEN_SET_SIZE];
unsigned char ignore = 0;

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
unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_node);
unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *goal, char goal_x, char goal_y);

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
    unsigned char *goal_position = &global_map[goal_x][goal_y];
    unsigned char *robot_position = &global_map[robot_x][robot_y];
    
    char g = 0;
    char h = 0;
    char f = g + h;
    
    //setup robot/goal position, clear open and closed sets
    initialisePointersNULL(closed_set, CLOSED_SET_SIZE);
    initialisePointersNULL(open_set, OPEN_SET_SIZE);

    //writeGlobalMap(ROBOT, robot_x, robot_y);
    //writeGlobalMap(GOAL, goal_x, goal_y);

    writeGlobalMap(f, robot_x, robot_y);                //put starting node (robot) to 0.
    open_set[0] = robot_position;                       //put starting node on open set list

    char count = 0;
    while (count <= 50) {
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
        *neighbour.up       = checkNeighbour(neighbour.up, goal_position, goal_x, goal_y);
        *neighbour.right    = checkNeighbour(neighbour.right, goal_position, goal_x, goal_y);
        *neighbour.down     = checkNeighbour(neighbour.down, goal_position, goal_x, goal_y);
        *neighbour.left     = checkNeighbour(neighbour.left, goal_position, goal_x, goal_y);

        //put current_open_set onto closed set
        pushToClosedSet(current_open_set);
        
        count++;
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
    for (char x = 0; x != GLOBAL_X; x++) {
        for (char y = 0; y != GLOBAL_Y; y++) {
            if (current_node == &global_map[x][y]) {
                if (x == 0 && neighbour_direction == UP)    return &ignore;
                if (x == 4 && neighbour_direction == DOWN)  return &ignore;
                if (y == 0 && neighbour_direction == LEFT)  return &ignore;
                if (y == 5 && neighbour_direction == RIGHT) return &ignore;

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

unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *goal, char goal_x, char goal_y) {
    char distance=0;
    char distance_x=0;
    char distance_y=0;
    char pos_x=0;
    char pos_y=0;

    if (neighbour == &ignore) return MAX;
    if (neighbour == goal) return 0;
    
    for (char x = 0; x != GLOBAL_X; x++) {
        for (char y = 0; y != GLOBAL_Y; y++) {
            if (neighbour == &global_map[x][y]) {
                pos_x = x;
                pos_y = y;
                break;
            }
        }
    }

    distance_x = pos_x - goal_x;
    distance_y = pos_y - goal_y;
    distance = abs(distance_x) + abs(distance_y);

    

    //if a node with same position as successor is in the OPEN list, which has lower distance value, skip it
    //for (char i = 0; i < OPEN_SET_SIZE; i++) {
    //    if (neighbour == open_set[i]) {
    //        if (*neighbour < &open_set[i]
    //    }
    //}
    //for (char i = 0; i < CLOSED_SET_SIZE; i++) {
    //    if *neighbour == closed_set[i]) {

    //    }
    //}
    pushToOpenSet(neighbour);

    return distance;
}
