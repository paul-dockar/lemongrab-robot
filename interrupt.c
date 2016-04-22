#include "interrupt.h"

volatile unsigned int rtc_counter = 0;
volatile bit pb_release = 0;
volatile unsigned char pb_debounce_count = 0;

 //setup PIC interrupt registers, control heartbeat and PB inputs
void interrupt isr(void) {
    //Serial interrupt handler
    ser_int();
    
    if (T0IF) {
        T0IF = 0; 
        TMR0 = TMR0_VAL;
        rtc_counter++;
        
        if (rtc_counter % 500 == 0) {        //cycle LED at 1Hz
            HB_LED = !HB_LED;
            rtc_counter = 0;
        }
        
        if (PB_START|PB_SCAN||PB_DRIVE_4M||PB_DRVE_SQUARE||PB_FIND_WALL){                  //check for PB inputs
            pb_debounce_count++;
            if (debounce(pb_debounce_count) && PB_START) {
                pb_start_pressed = 1;
                pb_release = 0;
            }
            if (debounce(pb_debounce_count) && PB_SCAN) {   //call debounce & PB check
                pb_scan_pressed = 1;                       //PB flag to ON
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

//debounces buttons
bit debounce (unsigned char count) {
    if (count > DEBOUNCE_REQ_COUNT && pb_release) {
        return 1;
    }
    else return 0;
}
