#include "interrupt.h"

volatile unsigned int   hb_led_counter = 0;     //timer0 interrupt overflow counter, used for heartbeat led
volatile unsigned char  pb_debounce_count = 0;  //push button debounce counter
volatile bit            pb_release = 0;         //push button flag when no buttons are pressed

//sets up pic interrupt registers, also push button/led input/output for portB
void setupInterrupt(void) {
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
        hb_led_counter++;

        if (hb_led_counter % 500 == 0) {
            HB_LED = !HB_LED;
            hb_led_counter = 0;
        }

        if (PB_SCAN||PB_DRIVE_4M||PB_DRIVE_SQUARE||PB_FIND_WALL){
            pb_debounce_count++;
            if (debounce(pb_debounce_count) && PB_SCAN) {
                pb_release = 0;
                pb_debounce_count = 0;
                pb_scan_pressed = 1;
            } else if (debounce(pb_debounce_count) && PB_DRIVE_4M) {
                pb_release = 0;
                pb_debounce_count = 0;
                pb_drive_4m_pressed = 1;
            } else if (debounce(pb_debounce_count) && PB_DRIVE_SQUARE) {
                pb_release = 0;
                pb_debounce_count = 0;
                pb_drive_square_pressed = 1;
            } else if (debounce(pb_debounce_count) && PB_FIND_WALL) {
                pb_release = 0;
                pb_debounce_count = 0;
                pb_find_wall_pressed = 1;
            } else {
                NOP();
            }
        }
        else {
            pb_debounce_count = 0;
            pb_release = 1;
        }
    }
}

//takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT
bit debounce(unsigned char count) {
    if (count > DEBOUNCE_REQ_COUNT && pb_release) {
        return 1;
    }
    else return 0;
}
