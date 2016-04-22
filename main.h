/* 
 * File:   main.h
 * Author: Paul
 *
 * Created on 6 April 2016, 11:43 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#pragma config BOREN = OFF, CPD = OFF, WRT = OFF, FOSC = HS, WDTE = OFF, CP = OFF, LVP = OFF, PWRTE = OFF 
#include "lcd.h"
#include "stepper.h"
#include "adc.h"
#include "interrupt.h"

#define _XTAL_FREQ 20000000     //PIC crystal frequency
#define LED0    RB0             //LED0(heartbeat) is set to PortB pin 0
#define LED1    RB1             //LED1 is set to PortB pin 1
#define PBLED   !RB2            //PBLED (to turn on LED1) is set to PortB pin 2
#define PBCW    !RB3            //PBCW (turn stepper CW) is set to PortB pin 3
#define PBCCW   !RB4            //PBCCW (turn stepper CCW) is set to PortB pin 4
#define PBROT   !RB5            //PBROT (turn stepper 180) is set to PortB pin 5

//The four push button flags
volatile bit pbLEDPressed;
volatile bit pbCWPressed;
volatile bit pbCCWPressed;
volatile bit pbROTPressed;

void setup (void);          //setup PIC, enable interrupts
void main (void);           //calls all setup functions, loops button checks and ADC
void buttonControl (void);  //checks for button flags, performs actions if TRUE

#endif	/* MAIN_H */