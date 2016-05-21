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
unsigned char *getNeighbourNodes(unsigned char *, unsigned char );
unsigned char checkNeighbour(unsigned char *, unsigned char *, unsigned char *, char , char );
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

//function takes the robot and goal positions and determines the path to travel next using the A* algorithm.
//function will return either -1 to 4 to handle all situations. these numbers are defined in the header
signed char findPathAStar(char robot_x, char robot_y, char goal_x, char goal_y) {
    unsigned char *current_open_set = 0;
    unsigned char *goal_position = &global_map[goal_x][goal_y];
    unsigned char *robot_position = &global_map[robot_x][robot_y];
    unsigned char direction_to_travel = 0;
    unsigned char iteration = 0;
    unsigned char smallest_open_set = 0;

	//TODO: more flags may be added in here
    if (goal_position == robot_position) { 						//check if robot is in goal positon
		return STOP;
	}

    setupGlobalMap();											//setup robot/goal position, clear open and closed sets, clear global map
    writeGlobalMap(ROBOT, robot_x, robot_y);
    writeGlobalMap(GOAL, goal_x, goal_y);
    initialisePointersNULL(closed_set, CLOSED_SET_SIZE);
    initialisePointersNULL(open_set, OPEN_SET_SIZE);

    marryUpLocalMapData(local, robot_x, robot_y);				//write relevent data from the IR scan (local_map) to the global map

	//after the intial setup, the below while loop is the main part of the A* algorithm.
	//in the ideal situation it will stop searching when the goal is found
	//put starting node on open set list to start the algorithm
    pushToOpenSet(robot_position);
    while (open_set [0] != 0) {
        smallest_open_set = WALL;
        for (char i = 0; i < OPEN_SET_SIZE; i++) {							//find node with least f on the open list
            if (open_set [i] != 0 && *open_set [i] < smallest_open_set) {
                smallest_open_set = *open_set [i];
                current_open_set = open_set [i];
            }
        }

        removeFromOpenSet(current_open_set);								//remove the chosen one from the open list

        neighbour.up    = getNeighbourNodes(current_open_set, UP);			//generate 4 neighbours from current node
        neighbour.right = getNeighbourNodes(current_open_set, RIGHT);
        neighbour.down  = getNeighbourNodes(current_open_set, DOWN);
        neighbour.left  = getNeighbourNodes(current_open_set, LEFT);

        //for each neighbour; write f value to global map
        *neighbour.up       = checkNeighbour(neighbour.up, goal_position, robot_position, goal_x, goal_y);
        *neighbour.right    = checkNeighbour(neighbour.right, goal_position, robot_position, goal_x, goal_y);
        *neighbour.down     = checkNeighbour(neighbour.down, goal_position, robot_position, goal_x, goal_y);
        *neighbour.left     = checkNeighbour(neighbour.left, goal_position, robot_position, goal_x, goal_y);

        if (iteration == 0) direction_to_travel = findDirectionToTravel(neighbour);	//set direction in first iteration.

        if (*neighbour.up       == GOAL) return direction_to_travel;				//exit when neighbour is the goal
        if (*neighbour.right    == GOAL) return direction_to_travel;
        if (*neighbour.down     == GOAL) return direction_to_travel;
        if (*neighbour.left     == GOAL) return direction_to_travel;

        pushToClosedSet(current_open_set); 										 	//put current_open_set onto closed set
        rearrangeOpenSet();                 										//make sure open_set[0] always has a value

        iteration++;
    }
    return direction_to_travel;
}

//takes the current node and desired direction to return a pointer to the neighbouring node on the global_map
//relative to the current node and direction. includes a quick check for out of bound conditions
unsigned char *getNeighbourNodes(unsigned char *current_node, unsigned char neighbour_direction) {
    for (char x = 0; x < GLOBAL_X; x++) {
        for (char y = 0; y < GLOBAL_Y; y++) {
            if (current_node == &global_map[x][y]) {
                if (x == 0              && neighbour_direction == UP)    return &ignore;
                if (x == (GLOBAL_X - 1) && neighbour_direction == DOWN)  return &ignore;
                if (y == 0              && neighbour_direction == LEFT)  return &ignore;
                if (y == (GLOBAL_Y - 1) && neighbour_direction == RIGHT) return &ignore;

                switch (neighbour_direction) {
                    case UP:    x--; return (&global_map[x][y]);
                    case RIGHT:	y++; return (&global_map[x][y]);
                    case DOWN:  x++; return (&global_map[x][y]);
                    case LEFT:  y--; return (&global_map[x][y]);
                }
            }
        }
    }
    return &ignore;
}

//returns the fScore for the neighbour node that is input into this function
//includes checks if neighbour is a special item on the global map
//includes checks if neighbour is already on the closed or open lists
unsigned char checkNeighbour(unsigned char *neighbour, unsigned char *goal, unsigned char *robot, char goal_x, char goal_y) {
    char fScore = 0;
    char gScore = 1;
    char hScore_x = 0;
    char hScore_y = 0;
    char pos_x = 0;
    char pos_y = 0;
    neighbour_already_on_list_flag = 0;

    if (*neighbour == DEADEND)  return DEADEND;
    if (*neighbour == WALL)     return WALL;	//wall check needs to beefore the rest
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

	//fScore (distance) calculation.
	//gScore should ideally be equal to the parent node - neighbour node distance,
	//but due to the layout of the map, each node is always 1m from eachother, so gScore is a constant value (1m)
	//hScore is determined by the distance from the neighbour node to the goal node
    hScore_x = pos_x - goal_x;
    hScore_y = pos_y - goal_y;
    fScore = gScore + abs_char(hScore_x) + abs_char(hScore_y);

    for (char i = 0; i < OPEN_SET_SIZE; i++) {    	//if a node with same position as successor is in the OPEN list, skip it
        if (neighbour == open_set[i]) {
            neighbour_already_on_list_flag = 1;
        }
    }
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {	//if a node with same position as successor is in the CLOSE list, skip it
        if (neighbour == closed_set[i]) {
            neighbour_already_on_list_flag = 1;
        }
    }
    if (!neighbour_already_on_list_flag) pushToOpenSet(neighbour);	//otherwise add node to open list

    return fScore;
}

//due to the layout of the map and the fact direction can change every single node, the direction to travel is determined
//in the first iteration. So this function takes the first neighbour set and determines which way to go.
char findDirectionToTravel(struct NEIGHBOUR neighbour) {
    unsigned char neighbour_travel[4];
    unsigned char lowest_travel = 255;
    char direction = 0;

    neighbour_travel[0] = *neighbour.up;
    neighbour_travel[1] = *neighbour.right;
    neighbour_travel[2] = *neighbour.down;
    neighbour_travel[3] = *neighbour.left;

    for (char i = 0; i < 4; i++){					//Checks for lowest values in neighbours, and saves it for direction to travel.
        if (neighbour_travel[i] == 200) {
            return i + 1;
        } else if (neighbour_travel[i] < 100 && neighbour_travel[i] < lowest_travel) {
            lowest_travel = neighbour_travel[i];
            direction = i + 1;
        }
    }

    if (lowest_travel > 250) direction = TURN_AROUND;	//if stuff is everywhere then its a deadend!

    return direction; 								//direction is either 1 (up), 2 (right), 3 (down), 4 (left), or -1 (dead-end)
}

//This function will write relevent data from the IR scan (local_map) to the global map before the A* algorithm is run
//It is abit of spaghetti but unfortunately necessary because the global_map is only 20 bytes instead of the ideal 99+ bytes
void marryUpLocalMapData(struct LOCAL local, char robot_x, char robot_y) {
    local.forward         = &local_map[0][1];
    local.right           = &local_map[1][2];
    local.back            = &local_map[2][1];
    local.left            = &local_map[1][0];
    local.robot_direction = local_map[1][1];

    #define     robot_x_min_check()     (robot_x > 0)
    #define     robot_y_min_check()     (robot_y > 0)
    #define     robot_x_max_check()     (robot_x < 3)
    #define     robot_y_max_check()     (robot_y < 4)

    #define     forward_wall_check()    (*local.forward  < 100 || *local.forward == 250)
    #define     right_wall_check()      (*local.right    < 100 || *local.right   == 250)
    #define     back_wall_check()       (*local.back     < 100 || *local.back    == 250)
    #define     left_wall_check()       (*local.left     < 100 || *local.left    == 250)

    #define     x_minus_one()   global_map[robot_x - 1][robot_y]
    #define     y_minus_one()   global_map[robot_x][robot_y - 1]
    #define     x_plus_one()    global_map[robot_x + 1][robot_y]
    #define     y_plus_one()    global_map[robot_x][robot_y + 1]

    switch (local.robot_direction) {
        case UP:
            if (robot_x_min_check() && forward_wall_check())    x_minus_one() = WALL;
            if (robot_y_max_check() && right_wall_check())      y_plus_one()  = WALL;
            if (robot_x_max_check() && back_wall_check())       x_plus_one()  = WALL;
            if (robot_y_min_check() && left_wall_check())       y_minus_one() = WALL;
            break;
        case RIGHT:
            if (robot_y_max_check() && forward_wall_check())    y_plus_one()  = WALL;
            if (robot_x_max_check() && right_wall_check())      x_plus_one()  = WALL;
            if (robot_y_min_check() && back_wall_check())       y_minus_one() = WALL;
            if (robot_x_min_check() && left_wall_check())       x_minus_one() = WALL;
            break;
        case DOWN:
            if (robot_x_max_check() && forward_wall_check())    x_plus_one()  = WALL;
            if (robot_y_min_check() && right_wall_check())      y_minus_one() = WALL;
            if (robot_x_min_check() && back_wall_check())       x_minus_one() = WALL;
            if (robot_y_max_check() && left_wall_check())       y_plus_one()  = WALL;
            break;
        case LEFT:
            if (robot_y_min_check() && forward_wall_check())    y_minus_one() = WALL;
            if (robot_x_min_check() && right_wall_check())      x_minus_one() = WALL;
            if (robot_y_max_check() && back_wall_check())       y_plus_one()  = WALL;
            if (robot_x_max_check() && left_wall_check())       x_plus_one()  = WALL;
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
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
        if (closed_set[i] == item_to_push) {
            return;
        }
    }
    for (char i = 0; i < CLOSED_SET_SIZE; i++) {
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
            open_set[i+1] = 0;
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
            if (global_map [x][y] == DEADEND) {
                continue;
            } else {
                global_map [x][y] = 0;
            }
        }
    }
}

void setupLocalMap(void) {
    for (char x = 0; x < LOCAL_X; x++) {
        for (char y = 0; y < LOCAL_Y; y++) {
            if (local_map [x][y] == local_map [1][1]) {
                continue;
            } else {
                local_map [x][y] = 250;
            }
        }
    }
}
