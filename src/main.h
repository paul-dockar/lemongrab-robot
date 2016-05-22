/*
 * Team:    Lemongrabs
 * Authors: Paul Dockar     - 12059869
 *          Dennis Kim      - 12050466
 *          Kim Izzo        - 11971278
 *          Nick Rabey      - 11988589
 *          Michael Smith   - 11745167
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#pragma config BOREN = OFF, CPD = OFF, WRT = OFF, FOSC = HS, WDTE = OFF, CP = OFF, LVP = OFF, PWRTE = OFF
#include "adc.h"
#include "eeprom.h"
#include "explore.h"
#include "interrupt.h"
#include "iRobot.h"
#include "lcd.h"
#include "ser.h"
#include "song.h"
#include "spi.h"
#include "stepper.h"

#define _XTAL_FREQ      20000000        //oscillator (Hz)
#define HB_LED          RB0             //set PortB Pin 0 for heartbeat LED
#define PB_EXPLORE      !RB1            //set PortB Pin 1 for pushbutton scan closest wall
#define PB_WALL_FOLLOW  !RB5            //set PortB Pin 4 for pushbutton drive along closest wall

volatile bit pb_explore_pressed;        //push button flag for PB_SCAN
volatile bit pb_find_wall_pressed;      //push button flag for PB_FIND_WALL

void setup(void);                       //calls all other module setup functions
void main(void);                        //main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void refreshLcd(int distance);          //Refresh the LCD with ADC value and distance value

#endif	/* MAIN_H */
