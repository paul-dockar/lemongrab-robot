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
void setupGlobalMap(void);
void setupLocalMap(void);
void writeGlobalMap(unsigned char, char, char);
char findPathAStar(char, char, char, char);
void initialisePointersNULL(unsigned char **, char);
void removeFromOpenSet(unsigned char *);
void pushToClosedSet(unsigned char *);
void pushToOpenSet(unsigned char *);
unsigned char *getNeighbourNodes(unsigned char *, unsigned char );
unsigned char checkNeighbour(unsigned char *, unsigned char *, unsigned char *, char , char );
unsigned char findDirectionToTravel(struct NEIGHBOUR);

void setupExplore(void) {
    setupGlobalMap();
    setupLocalMap();
}

void setupGlobalMap(void) {
    for (char x = 0; x < GLOBAL_X; x++) {
        for (char y = 0; y < GLOBAL_Y; y++) {
            global_map [x][y] = 0;
        }
    }
}

void setupLocalMap(void) {
    for (char x = 0; x < LOCAL_X; x++) {
        for (char y = 0; y < LOCAL_Y; y++) {
            local_map [x][y] = 0;
        }
    }
}

void writeGlobalMap(unsigned char value, char x, char y) {
    global_map [x][y] = value;
}

void writeLocalMap(unsigned int value, char x, char y) {
    local_map [x][y] = value;
}

char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    unsigned char *current_open_set = 0;
    unsigned char *goal_position = &global_map[goal_x][goal_y];
    unsigned char *robot_position = &global_map[robot_x][robot_y];
    unsigned char direction_to_travel = 0;
    unsigned char iteration = 0;

    //check if robot is in goal positon
    if (goal_position == robot_position) return 0;
    
    //setup robot/goal position, clear open and closed sets, clear global map
    setupGlobalMap();
    initialisePointersNULL(closed_set, CLOSED_SET_SIZE);
    initialisePointersNULL(open_set, OPEN_SET_SIZE);

    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);

    pushToOpenSet(robot_position);                       //put starting node on open set list

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

        //set direction in first iteration.
        if (iteration == 0) direction_to_travel = findDirectionToTravel(neighbour);

        //exit when neighbour is the goal
        if (*neighbour.up       == GOAL) return direction_to_travel;
        if (*neighbour.right    == GOAL) return direction_to_travel;
        if (*neighbour.down     == GOAL) return direction_to_travel;
        if (*neighbour.left     == GOAL) return direction_to_travel;

        //put current_open_set onto closed set
        pushToClosedSet(current_open_set);
        iteration++;
    }
    return direction_to_travel;
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

void pushToClosedSet(unsigned char *item_to_push) {
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (closed_set[i] == item_to_push) {
            break;
        } else if (closed_set[i] == 0) {
            closed_set[i] = item_to_push;
            break;
        }
    }
}

void pushToOpenSet(unsigned char *item_to_push) {
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set[i] == item_to_push) {
            break;
        } else if (open_set[i] == 0) {
            open_set[i] = item_to_push;
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
    char fScore = 0;
    char gScore = 1;
    char hScore_x = 0;
    char hScore_y = 0;
    char pos_x = 0;
    char pos_y = 0;
    char flag = 0;

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

    hScore_x = pos_x - goal_x;
    hScore_y = pos_y - goal_y;
    fScore = gScore + abs(hScore_x) + abs(hScore_y);

    //if a node with same position as successor is in the OPEN list, skip it
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (neighbour == open_set[i]) {
            flag = 1;
        }
    }
    //if a node with same position as successor is in the CLOSE list, skip it
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (neighbour == closed_set[i]) {
            flag = 1;
        }
    }
    //otherwise add node to open list
    if (flag != 1) pushToOpenSet(neighbour);

    return fScore;
}

unsigned char findDirectionToTravel(struct NEIGHBOUR neighbour) {
    unsigned char neighbour_travel[4];
    unsigned char lowest_travel = 255;
    unsigned char direction = 0;

    neighbour_travel[0] = *neighbour.up;
    neighbour_travel[1] = *neighbour.right;
    neighbour_travel[2] = *neighbour.down;
    neighbour_travel[3] = *neighbour.left;
    
    //the statement here checks for lowest values in neighbours, and saves it for direction to travel. 
    //note, because neighbour.left is checked LAST, and the sign <= is used, it is left justified.
    //therefore it chooses directions with same distance in order, left, down, right, up
    for (char i = 0; i < 4; i++){
        if (neighbour_travel[i] == 200) {
            return i + 1;
        } else if (neighbour_travel[i] < 100 && neighbour_travel[i] <= lowest_travel) {
            lowest_travel = neighbour_travel[i];
            direction = i + 1;
        }
    }

    return direction; //direction is either 1 (up), 2 (right), 3 (down), 4 (left) 
}
