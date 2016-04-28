#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

unsigned int scan_360_closest_step_count;   //counter to count how many half steps since closest scanned object
int closest_adc_distance;                   //variable to store closest reading of adc distance since push button press

unsigned char CW_control_byte;      //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char CCW_control_byte;     //stepper motor control byte for; enabled, counterclockwise, half-steps

void scan360(unsigned int steps);   //rotate stepper CW 360 degrees. scan adc distance each half step.
void findClosestWall(void);         //takes ADC and checks against old adc value, keeping the closest 'distance'
void moveCW(unsigned int steps);    //move stepper CW
void moveCCW(unsigned int steps);   //move stepper CCW
void resetADC(void);                //simple function to clear adc distance and step counters

#endif	/* STEPPER_H */
