#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include "main.h"

#define DEBOUNCE_REQ_COUNT  10      //push button debounce required counter
#define TMR0_VAL            100     //start timer0 at 100

volatile unsigned int   hb_led_counter;     //timer0 interrupt overflow counter, used for heartbeat led
volatile unsigned int   lost_wall_timer;    //lost wall timer for wallFollow mode. Is only reset during wallFollow
volatile unsigned char  pb_debounce_count;  //push button debounce counter
volatile bit            pb_release;         //push button flag when no buttons are pressed

void setupInterrupt(void);          //sets up pic interrupt registers, also push button/led input/output for portB
void interrupt isr(void);           //interrupt function for the PIC. Uses timer0 interrupt overflow. Controls heartbeat LED. Controls push button debouncing and setting of push button flags
bit debounce(unsigned char count);  //takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT

#endif	/* INTERRUPT_H */
