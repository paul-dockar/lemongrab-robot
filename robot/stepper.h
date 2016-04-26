#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

unsigned char controlByte;
unsigned short scan_360_ccw_step_count;
unsigned short new_adc_distance;
unsigned short closest_adc_distance;

void scan360 (unsigned short steps);
void moveCW (unsigned short steps);
void moveCCW (unsigned short steps);
void findClosestWall(void);
void resetADC(void);

#endif	/* STEPPER_H */
