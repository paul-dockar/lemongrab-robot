/*
 * File:   main.c
 * Author: Paul Dockar
 *
 * Created on 6 April 2016, 11:43 PM
 */

#include "main.h"

//PB flags, 180 degree rotate flag to switch between CW and CCW
volatile bit pbLEDPressed = 0;
volatile bit pbCWPressed = 0;
volatile bit pbCCWPressed = 0;
volatile bit pbROTPressed = 0;
volatile bit ROTATE_FLAG = 0;

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
    if (pbLEDPressed) {
        LED1 = !LED1;
        pbLEDPressed = 0;
    }
    if (pbCWPressed) {
        moveCW(1);
        pbCWPressed = 0;
    }
    if (pbCCWPressed) {
        moveCCW(1);
        pbCCWPressed = 0;
    }
    if (pbROTPressed) {
        if (ROTATE_FLAG && pbROTPressed) {
            moveCW(200);
            ROTATE_FLAG = !ROTATE_FLAG;
            pbROTPressed = 0;
        }
        if (!ROTATE_FLAG & pbROTPressed) {
            moveCCW(200);
            ROTATE_FLAG = !ROTATE_FLAG;
            pbROTPressed = 0;
        }
    }
}
