/* 
 * File:   main.h
 * Authors:     Lemongrabs
 *                  Paul Dockar     - 12059869
 *                  Dennis Kim      - 12050466
 *                  Kim Izzo        - 11971278
 *                  Nick Rabey      - 11988589
 *                  Michael Smith   - 11745167
 *
 * Created on 22 April 2016
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#pragma config BOREN = OFF, CPD = OFF, WRT = OFF, FOSC = HS, WDTE = OFF, CP = OFF, LVP = OFF, PWRTE = OFF 
#include "lcd.h"
#include "stepper.h"
#include "adc.h"
#include "interrupt.h"

#define _XTAL_FREQ      20000000     //PIC crystal frequency
#define LED0            RB0             //LED0(heartbeat) is set to PortB pin 0
#define UNUSED          RB1             //LED1 is set to PortB pin 1
#define PB_SCAN         !RB2            //PBLED (to turn on LED1) is set to PortB pin 2
#define PB_DRIVE_4M     !RB3            //PBCW (turn stepper CW) is set to PortB pin 3
#define PB_DRVE_SQUARE  !RB4            //PBCCW (turn stepper CCW) is set to PortB pin 4
#define PB_FIND_WALL    !RB5            //PBROT (turn stepper 180) is set to PortB pin 5

//The four push button flags
volatile bit pb_scan_pressed;
volatile bit pb_drive_4m_pressed;
volatile bit pb_drive_square_pressed;
volatile bit pb_find_wall_pressed;

void setup (void);          //setup PIC, enable interrupts
void main (void);           //calls all setup functions, loops button checks and ADC
void buttonControl (void);  //checks for button flags, performs actions if TRUE

#endif	/* MAIN_H */