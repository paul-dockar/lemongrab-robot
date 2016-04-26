#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

signed char cstep;
signed int stepper_count;
volatile bit cw_flag;
unsigned char controlByte;

unsigned short scan_360_ccw_step_count;
float new_adc_distance;
float closest_adc_distance;

void scan360 (unsigned short steps);
void moveCW (unsigned short steps);
void moveCCW (unsigned short steps);
void lcdCW (void);
void lcdCCW (void);
void writeStep (signed int stepCount);
void findClosestWall(void);
void resetADC(void);

#endif	/* STEPPER_H */
