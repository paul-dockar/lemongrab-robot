#include "main.h"

volatile bit pb_start_pressed = 0;
volatile bit pb_scan_pressed = 0;
volatile bit pb_drive_4m_pressed = 0;
volatile bit pb_drive_square_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

void setup (void) {
    __delay_ms(5000);           //start-up delay
    
    setupInterrupt();
    setupSPI();
    setupADC();
    setupLCD();
    setupIRobot();
}

void main (void) {
    setup();
    while (1) {
        buttonControl();
        adcDisplay();
    }
}

void buttonControl (void) {
    if (pb_start_pressed) {
        startTest();
        pb_start_pressed = 0;
    }
    if (pb_scan_pressed) {
        scan360(100);
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

void startTest (void) {
        drive(DRIVE,0,200,0,200);
        __delay_ms(1000);
        drive(DRIVE,0,50,0,50);
        __delay_ms(4000);
        stop();
        figureEightTest();
        __delay_ms(5000);
        stop();
}
