#include "main.h"

//set all push button flags to 0
volatile bit pb_scan_pressed = 0;
volatile bit pb_drive_4m_pressed = 0;
volatile bit pb_drive_square_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//calls all other module setup functions
void setup(void) {
    __delay_ms(5000);               //5 second startup delay to filter out irobot create serial crap

    setupInterrupt();               //calls pic interrupt setup function
    setupIRobot();                  //calls irobot create setup function
    setupSPI();                     //calls spi setup function for stepper motor control
    setupADC();                     //calls adc setup function for ir reading control
    setupLCD();                     //calls lcd setup function to allow writing to lcd
}

//main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void main(void) {
    setup();
    while (1) {
        buttonControl();
        refreshLcd(total_distance_travel);
    }
}

//checks all push button flags. if one is true then call that desired function
void buttonControl(void) {
    if (pb_scan_pressed) {
        wallFollow();
        pb_scan_pressed = 0;
    }
    if (pb_drive_4m_pressed) {
        moveStraight();
        pb_drive_4m_pressed = 0;
    }
    if (pb_drive_square_pressed) { //this currently does not work
        moveSquare();
        pb_drive_square_pressed = 0;
    }
    if (pb_find_wall_pressed) {
        moveSquare();
        pb_find_wall_pressed = 0;
    }
}

void refreshLcd(int distance) {
    adcDisplay();
    distanceDisplay(distance);
}
