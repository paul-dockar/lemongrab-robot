#include "explore.h"

unsigned char *closed_set [CLOSED_SET_SIZE];
unsigned char *open_set [OPEN_SET_SIZE];
unsigned char ignore = 0;
bit neighbour_already_on_list_flag;

struct NEIGHBOUR {
    unsigned char *up;
    unsigned char *right;
    unsigned char *down;
    unsigned char *left;
} neighbour;

struct LOCAL {
    unsigned char *forward;
    unsigned char *right;
    unsigned char *back;
    unsigned char *left;
    unsigned char robot_direction;
} local;

//declare local functions
char findPathAStar(char, char, char, char);
unsigned char *getNeighbourNodes(unsigned char *, unsigned char );
unsigned char checkNeighbour(unsigned char *, unsigned char *, unsigned char *, unsigned char *, char , char );
char findDirectionToTravel(struct NEIGHBOUR);
void marryUpLocalMapData(struct LOCAL, char, char);
void initialisePointersNULL(unsigned char **, char);
void removeFromOpenSet(unsigned char *);
void pushToClosedSet(unsigned char *);
void pushToOpenSet(unsigned char *);
void rearrangeOpenSet(void);
void writeGlobalMap(unsigned char, char, char);
void setupGlobalMap(void);
void setupLocalMap(void);

void setupExplore(void) {
    setupGlobalMap();
    setupLocalMap();
}

char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    unsigned char *current_open_set = 0;
    unsigned char *goal_position = &global_map[goal_x][goal_y];
    unsigned char *robot_position = &global_map[robot_x][robot_y];
    unsigned char direction_to_travel = 0;
    unsigned char iteration = 0;
    unsigned char smallest_open_set = 0;

    //check if robot is in goal positon
    if (goal_position == robot_position) return 0;
    
    //setup robot/goal position, clear open and closed sets, clear global map
    setupGlobalMap();
    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);
    initialisePointersNULL(closed_set, CLOSED_SET_SIZE);
    initialisePointersNULL(open_set, OPEN_SET_SIZE);

    marryUpLocalMapData(local, robot_x, robot_y);
    pushToOpenSet(robot_position);                       //put starting node on open set list

    while (*open_set [0] != 0) {
        //find node with least f on the open list
        smallest_open_set = WALL;
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
        *neighbour.up       = checkNeighbour(neighbour.up, local.forward, goal_position, robot_position, goal_x, goal_y);
        *neighbour.right    = checkNeighbour(neighbour.right, local.right, goal_position, robot_position, goal_x, goal_y);
        *neighbour.down     = checkNeighbour(neighbour.down, local.back, goal_position, robot_position, goal_x, goal_y);
        *neighbour.left     = checkNeighbour(neighbour.left, local.left, goal_position, robot_position, goal_x, goal_y);

        //set direction in first iteration.
        if (iteration == 0) direction_to_travel = findDirectionToTravel(neighbour);
        if (direction_to_travel == 255) return direction_to_travel;

        //exit when neighbour is the goal
        if (*neighbour.up       == GOAL) return direction_to_travel;
        if (*neighbour.right    == GOAL) return direction_to_travel;
        if (*neighbour.down     == GOAL) return direction_to_travel;
        if (*neighbour.left     == GOAL) return direction_to_travel;
        
        pushToClosedSet(current_open_set);  //put current_open_set onto closed set
        rearrangeOpenSet();                 //make sure open_set[0] always has a value

        iteration++;
    }
    return -1;
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

unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *local, unsigned char *goal, unsigned char *robot, char goal_x, char goal_y) {
    char fScore = 0;
    char gScore = 1;
    char hScore_x = 0;
    char hScore_y = 0;
    char pos_x = 0;
    char pos_y = 0;
    
    neighbour_already_on_list_flag = 0;

    //wall check needs to beefore the rest
    if (*neighbour == WALL)     return WALL;

    if (neighbour == &ignore)   return WALL;
    if (neighbour == goal)      return GOAL;
    if (neighbour == robot)     return ROBOT;
    
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
            neighbour_already_on_list_flag = 1;
        }
    }
    //if a node with same position as successor is in the CLOSE list, skip it
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (neighbour == closed_set[i]) {
            neighbour_already_on_list_flag = 1;
        }
    }
    //otherwise add node to open list
    if (neighbour_already_on_list_flag != 1) pushToOpenSet(neighbour);

    return fScore;
}

char findDirectionToTravel(struct NEIGHBOUR neighbour) {
    unsigned char neighbour_travel[4];
    unsigned char lowest_travel = 255;
    char direction = 0;

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
        } else if (neighbour_travel[i] < 100 && neighbour_travel[i] < lowest_travel) {
            lowest_travel = neighbour_travel[i];
            direction = i + 1;
        }
    }

    //if walls are everywhere then its a deadend!
    if (lowest_travel > 250) direction = -1;

    return direction; //direction is either 1 (up), 2 (right), 3 (down), 4 (left), or -1 for dead-end
}

void marryUpLocalMapData(struct LOCAL local, char robot_x, char robot_y) {
    local.forward         = &local_map[0][1];
    local.right           = &local_map[1][2];
    local.back            = &local_map[2][1];
    local.left            = &local_map[1][0];
    local.robot_direction = local_map[1][1];

    #define     X_minus_1()   global_map[robot_x - 1][robot_y]
    #define     Y_minus_1()   global_map[robot_x][robot_y - 1]
    #define     X_plus_1()    global_map[robot_x + 1][robot_y]
    #define     Y_plus_1()    global_map[robot_x][robot_y + 1]

    #define     forward_wall_check()    (*local.forward  < 100 || *local.forward >= 250)
    #define     right_wall_check()      (*local.right    < 100 || *local.right   >= 250)
    #define     back_wall_check()       (*local.back     < 100 || *local.back    >= 250)
    #define     left_wall_check()       (*local.left     < 100 || *local.left    >= 250)

    switch (local.robot_direction) {
        case UP:
            if (robot_x > 0 && forward_wall_check())    X_minus_1() = WALL;
            if (robot_y > 0 && right_wall_check())      Y_plus_1()  = WALL;
            if (robot_x < 3 && back_wall_check())       X_plus_1()  = WALL;
            if (robot_y < 4 && left_wall_check())       Y_minus_1() = WALL;
            break;
        case RIGHT:
            if (robot_x > 0 && forward_wall_check())    Y_plus_1()  = WALL;
            if (robot_y > 0 && right_wall_check())      X_plus_1()  = WALL;
            if (robot_x < 3 && back_wall_check())       Y_minus_1() = WALL;
            if (robot_y < 4 && left_wall_check())       X_minus_1() = WALL;
            break;
        case DOWN:
            if (robot_x > 0 && forward_wall_check())    X_plus_1()  = WALL;
            if (robot_y > 0 && right_wall_check())      Y_minus_1() = WALL;
            if (robot_x < 3 && back_wall_check())       X_minus_1() = WALL;
            if (robot_y < 4 && left_wall_check())       Y_plus_1()  = WALL;
            break;
        case LEFT:
            if (robot_x > 0 && forward_wall_check())    Y_minus_1() = WALL;
            if (robot_y > 0 && right_wall_check())      X_minus_1() = WALL;
            if (robot_x < 3 && back_wall_check())       Y_plus_1()  = WALL;
            if (robot_y < 4 && left_wall_check())       X_plus_1()  = WALL;
            break;
    }
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
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (closed_set[i] == item_to_push) {
            return;
        }
    }
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (closed_set[i] == 0) {
            closed_set[i] = item_to_push;
            return;
        }
    }
}

void pushToOpenSet(unsigned char *item_to_push) {
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set[i] == item_to_push) {
            return;
        }
    }
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set[i] == 0) {
            open_set[i] = item_to_push;
            return;
        }
    }
}

void rearrangeOpenSet(void) {
    for (char i = 0; i < OPEN_SET_SIZE; i++) {
        if (open_set[i] == 0 && i < (OPEN_SET_SIZE - 1)) {
            open_set[i] = open_set[i+1];
        }
    }
}

void writeGlobalMap(unsigned char value, char x, char y) {
    global_map [x][y] = value;
}

void writeLocalMap(unsigned char value, char x, char y) {
    local_map [x][y] = value;
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
            local_map [x][y] = 250;
        }
    }
}
