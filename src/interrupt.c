#include "interrupt.h"

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
        ir_move_timer++;

        //toggles heartbeat LED every 0.5 seconds
        if (hb_led_counter % 500 == 0) {
            HB_LED = !HB_LED;
            hb_led_counter = 0;
        }

        //checks for all 4 push buttons. If one is detected as pressed, start debounce counter. If debounce counter returns 1, then toggle push button flag.
        if (PB_EXPLORE){
            pb_debounce_count++;
            if (debounce(pb_debounce_count) && PB_EXPLORE) {
                pb_release = 0;
                pb_explore_pressed = 1;
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
