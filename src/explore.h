#ifndef EXPLORE_H
#define	EXPLORE_H

#include "main.h"

void setupExplore(void);
void setupGlobalMap(void);
void setupLocalMap(void);
void writeLocalMap(int, char, char);
void getLocalMap(char, char);

bit still_exploring;
char global_map [4][5];
char local_map  [3][3];

#endif	/* EXPLORE_H */
