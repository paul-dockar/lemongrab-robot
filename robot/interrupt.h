#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include "main.h"

#define DEBOUNCE_REQ_COUNT 10               //pushbutton debounce required counter
#define TMR0_VAL 100                        //tmr0 overflow counter

volatile unsigned int rtc_counter;          //timer0 interrupt overflow counter, used for heartbeat led
volatile bit pb_release;                    //push button flag when no buttons are pressed
volatile unsigned char pb_debounce_count;   //push button debounce counter

void setupInterrupt (void);                 //sets up pic interrupt registers, also push button/led input/output for portB
void interrupt isr (void);                  //interrupt function for the PIC. Uses timer0 interrupt overflow. Controls heartbeat LED. Controls push button debouncing and setting of push button flags
bit debounce (unsigned char count);         //takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT

#endif	/* INTERRUPT_H */
