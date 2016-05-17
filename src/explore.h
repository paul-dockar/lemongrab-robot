#ifndef EXPLORE_H
#define	EXPLORE_H

#include "main.h"

#define     ROBOT       100
#define     GOAL        200
#define     MAX         255
#define     DEADEND     254
#define     CLIFF       253

#define     GLOBAL_X    4
#define     GLOBAL_Y    5
#define     LOCAL_X     3
#define     LOCAL_Y     3

#define     UP          1
#define     RIGHT       2
#define     DOWN        3
#define     LEFT        4
void setupExplore(void);
void setupGlobalMap(void);
void setupLocalMap(void);
void writeGlobalMap(unsigned char, char, char);
void writeLocalMap(unsigned int, char, char);
char findPathAStar(char, char, char, char);

bit exploring;
char global_map [4][5];
char local_map  [3][3];

#endif	/* EXPLORE_H */
