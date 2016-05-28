#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

#define     FULL_SCAN       8
#define     HALF_SCAN       5

void moveCW(unsigned int);    //move stepper CW
void moveCCW(unsigned int);   //move stepper CCW
void scanLocal(char scan_steps);

#endif	/* STEPPER_H */
