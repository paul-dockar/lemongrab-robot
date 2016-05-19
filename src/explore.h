#ifndef EXPLORE_H
#define	EXPLORE_H

#include "main.h"

#define     ROBOT       100
#define     GOAL        200
#define     MAX         255
#define     DEADEND     254
#define     CLIFF       253

#define     GLOBAL_X        4
#define     GLOBAL_Y        5
#define     LOCAL_X         3
#define     LOCAL_Y         3

#define     CLOSED_SET_SIZE 10
#define     OPEN_SET_SIZE   10

#define     UP              1
#define     RIGHT           2
#define     DOWN            3
#define     LEFT            4

void setupExplore(void);
char findPathAStar(char, char, char, char);
void writeLocalMap(unsigned int, char, char);

unsigned char global_map [GLOBAL_X][GLOBAL_Y];
unsigned char local_map  [LOCAL_X][LOCAL_Y];

#endif	/* EXPLORE_H */
