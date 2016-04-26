#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

#define SM_CLOCKWISE            0b00001111
#define SM_COUNTER_CLOCKWISE    0b00001101

unsigned char controlByte;
unsigned short scan_360_ccw_step_count;
unsigned short new_adc_distance;
unsigned short closest_adc_distance;

unsigned char CW_control_byte;
unsigned char CCW_control_byte;

void scan360 (unsigned short steps);
void moveCW (unsigned short steps);
void moveCCW (unsigned short steps);
void findClosestWall(void);
void resetADC(void);

#endif	/* STEPPER_H */
