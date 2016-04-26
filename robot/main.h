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
#include "lcd.h"
#include "stepper.h"
#include "adc.h"
#include "interrupt.h"
#include "ser.h"
#include "iRobot.h"
#include "spi.h"

#define _XTAL_FREQ      20000000
#define HB_LED          RB0
#define PB_START        !RB1
#define PB_SCAN         !RB2
#define PB_DRIVE_4M     !RB3
#define PB_DRVE_SQUARE  !RB4
#define PB_FIND_WALL    !RB5

volatile bit pb_start_pressed;
volatile bit pb_scan_pressed;
volatile bit pb_drive_4m_pressed;
volatile bit pb_drive_square_pressed;
volatile bit pb_find_wall_pressed;

void setup (void);
void main (void);
void buttonControl (void);
void startTest (void);

#endif	/* MAIN_H */
