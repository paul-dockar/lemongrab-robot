#include "main.h"

//PB flags, 180 degree rotate flag to switch between CW and CCW
volatile bit pb_start_pressed = 0;
volatile bit pb_scan_pressed = 0;
volatile bit pb_drive_4m_pressed = 0;
volatile bit pb_drive_square_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//setup PIC, enable interrupts
void setup (void) {
    __delay_ms(5000);   // Safety delay while iRobot serial buffers are streaming stuff
    ser_init();
    PEIE = 1;
    GIE = 1;
    TRISB = 0b00111110;

    OPTION_REG = 0b00000100;
    TMR0IE = 1;                 //enable timer 0
    ei();                       //enable interrupt

    setupIRobot();
    setupADC();
    setupLCD();
}

//calls all setup functions, loops button checks and ADC
void main (void) {
    setup();

    while (1) {
        buttonControl();
        adcDisplay();
    }
}

//checks for button flags, performs actions if TRUE
void buttonControl (void) {
    if (pb_start_pressed) {
        RB6 = !RB6;
        start();
        pb_start_pressed = 0;
    }
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

//test code to check robot. Drive forward for 10 seconds, figure 8 for 10 seconds
void start (void) {
        drive();
        __delay_ms(10000);
        stop();
        figureEightTest();
        __delay_ms(10000);
        stop();
}
