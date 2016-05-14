#ifndef EXPLORE_H
#define	EXPLORE_H

#include "main.h"

#define     ROBOT       100
#define     GOAL        200
#define     WALL        255
#define     DEADEND     254
#define     CLIFF       253

void setupExplore(void);
void setupGlobalMap(void);
void setupLocalMap(void);
void writeGlobalMap(char, char, char);
void writeLocalMap(int, char, char);
char findPathAStar(char, char, char, char);

bit exploring;
char global_map [4][5];
char local_map  [3][3];

#endif	/* EXPLORE_H */
