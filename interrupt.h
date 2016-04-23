#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include "main.h"

#define DEBOUNCE_REQ_COUNT 10
#define TMR0_VAL 100

volatile unsigned int rtc_counter;       //heartbeat counter
volatile bit pb_release;                 //PB release flag
volatile unsigned char pb_debounce_count; //PB debounce counter

void interrupt isr(void);               //setup PIC interrupt registers, control heartbeat and PB inputs
bit debounce (unsigned char count);     //debounces buttons
        
#endif	/* INTERRUPT_H */
