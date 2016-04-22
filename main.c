/*
 * File:   main.c
 * Author: Paul Dockar
 *
 * Created on 6 April 2016, 11:43 PM
 */

#include "main.h"

//PB flags, 180 degree rotate flag to switch between CW and CCW
volatile bit pb_scan_pressed = 0;
volatile bit pb_drive_4m_pressed = 0;
volatile bit pb_drive_square_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//setup PIC, enable interrupts
void setup (void) {
    TRISB = 0b00111100;         //set pins 2-5 to input, rest output
    TRISC = 0b00000000;         //set all pins to output
        
    OPTION_REG = 0b00000100;
    TMR0 = TMR0_VAL;     
    TMR0IE = 1;                 //enable timer 0
    ei();                       //enable interrupt
}

//calls all setup functions, loops button checks and ADC
void main (void) {
    setup();
    setupADC();
    setupLCD();
    
    while (1) {
        buttonControl();
        adcDisplay();
    }
}

//checks for button flags, performs actions if TRUE
void buttonControl (void) {
    if (pb_scan_pressed) {
        scan360(400);
        pb_scan_pressed = 0;
    }
    if (pb_drive_4m_pressed) {
        moveCW(100);
        pb_drive_4m_pressed = 0;
    }
    if (pb_drive_square_pressed) {
        moveCCW(100);
        pb_drive_square_pressed = 0;
    }
    if (pb_find_wall_pressed) {
        moveCCW(8);
        pb_find_wall_pressed = 0;
    }
}
