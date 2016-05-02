#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

unsigned int scan_360_closest_step_count;   //counter to count how many half steps since closest scanned object
unsigned int old_adc_distance;              //variable to store closest reading of adc distance since push button press
unsigned char cw_control_byte;              //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char ccw_control_byte;             //stepper motor control byte for; enabled, counterclockwise, half-steps

void scanCw(unsigned int steps);    //Scan stepper motor clockwise. Finds the closest wall and stores this value to return to position later. Displays the distance per step to the LCD
void scanCcw(unsigned int steps);   //Scan stepper motor counter clockwise. Finds the closest wall and stores this value to return to position later. Displays the distance per step to the LCD
void findClosestWall(void);         //takes ADC and checks against old adc value, keeping the closest 'distance'
void moveCW(unsigned int steps);    //move stepper CW
void moveCCW(unsigned int steps);   //move stepper CCW
void resetADC(void);                //simple function to clear adc distance and step counters

#endif	/* STEPPER_H */
