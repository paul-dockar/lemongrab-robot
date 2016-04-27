#include "interrupt.h"

volatile unsigned int rtc_counter = 0;              //timer0 interrupt overflow counter, used for heartbeat led
volatile bit pb_release = 0;                        //push button flag when no buttons are pressed
volatile unsigned char pb_debounce_count = 0;       //push button debounce counter

//sets up pic interrupt registers, also push button/led input/output for portB
void setupInterrupt (void) {
    TRISB = 0b00111110;
    PEIE = 1;
    GIE = 1;

    OPTION_REG = 0b00000100;
    TMR0IE = 1;
    ei();
}

//Interrupt function for the PIC. Uses timer0 interrupt overflow
//Controls heartbeat LED
//Controls push button debouncing and setting of push button flags
void interrupt isr(void) {
    ser_int();

    if (T0IF) {
        T0IF = 0;
        TMR0 = TMR0_VAL;
        rtc_counter++;

        if (rtc_counter % 500 == 0) {
            HB_LED = !HB_LED;
            rtc_counter = 0;
        }

        if (PB_START||PB_SCAN||PB_DRIVE_4M||PB_DRVE_SQUARE||PB_FIND_WALL){
            pb_debounce_count++;
            //NOTE THIS PB IS TEMP TEST CODE
            if (debounce(pb_debounce_count) && PB_START) {
                pb_start_pressed = 1;
                pb_release = 0;
            }
            if (debounce(pb_debounce_count) && PB_SCAN) {
                pb_scan_pressed = 1;
                pb_release = 0;
            }
            if (debounce(pb_debounce_count) && PB_DRIVE_4M) {
                pb_drive_4m_pressed = 1;
                pb_release = 0;
            }
            if (debounce(pb_debounce_count) && PB_DRVE_SQUARE) {
                pb_drive_square_pressed = 1;
                pb_release = 0;
            }
            if (debounce(pb_debounce_count) && PB_FIND_WALL) {
                pb_find_wall_pressed = 1;
                pb_release = 0;
            }
        }
        else {
            pb_debounce_count = 0;
            pb_release = 1;
        }
    }
}

//takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT
bit debounce (unsigned char count) {
    if (count > DEBOUNCE_REQ_COUNT && pb_release) {
        return 1;
    }
    else return 0;
}
