/* 
 * File:   stepper.h
 * Author: Paul
 *
 * Created on 6 April 2016, 11:46 PM
 */

#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

//defines 8 half steps
#define STEP0 0b00111100 
#define STEP1 0b00101110 
#define STEP2 0b00101011 
#define STEP3 0b00100111 
#define STEP4 0b00110101 
#define STEP5 0b00010111 
#define STEP6 0b00011011 
#define STEP7 0b00011110

signed char cstep;                      //variable to count current step
signed int stepperCount;                //LCD step count
volatile bit cw_flag;                   //Flag for CW or CCW LCD display

void moveCW (char steps);               //move stepper CW
void moveCCW (char steps);              //move stepper CCW
void lcdCW (void);                      //updates LCD step position
void lcdCCW (void);                     //updates LCD step position
void writeStep (signed int stepCount);  //updates LCD step position

#endif	/* STEPPER_H */

