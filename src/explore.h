#ifndef EXPLORE_H
#define	EXPLORE_H

#include "main.h"

//global_map defined values
#define     ROBOT       100
#define     GOAL        200
#define     WALL        255
#define     DEADEND     254
#define     CLIFF       253
#define     BUMP        252
#define     VIRTWALL    251

//define local and global map array sizes
#define     GLOBAL_X        4
#define     GLOBAL_Y        5
#define     LOCAL_X         3
#define     LOCAL_Y         3

//define a* closed and open set sizes
#define     CLOSED_SET_SIZE 20
#define     OPEN_SET_SIZE   20

//define return values for astar direction to travel results
#define     TURN_AROUND     -1
#define     STOP            0
#define     UP              1
#define     RIGHT           2
#define     DOWN            3
#define     LEFT            4

void setupExplore(void);                            //function clears global & local map arrays
void setupLocalMap(void);                           //sets all values on local map to 250
signed char findPathAStar(char, char, char, char);  //takes robot and goal positions and returns direction to travel with a* algorithm
void writeLocalMap(unsigned char, char, char);      //function takes a value and the xy array position and writes it

unsigned char global_map[GLOBAL_X][GLOBAL_Y];
unsigned char local_map[LOCAL_X][LOCAL_Y];
volatile bit temp_global_info_flag;                 //this flag is used to not overwrite important information on the global_map for 1 iteration.

#endif	/* EXPLORE_H */
