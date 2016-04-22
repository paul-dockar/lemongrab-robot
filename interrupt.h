/* 
 * File:   interrupt.h
 * Author: Paul
 *
 * Created on 6 April 2016, 11:47 PM
 */

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include "main.h"

#define DEBOUNCE_REQ_COUNT 10
#define TMR0_VAL 100

volatile unsigned int rtcCounter;       //heartbeat counter
volatile bit pbRelease;                 //PB release flag
volatile unsigned char pbDebounceCount; //PB debounce counter

void interrupt isr(void);               //setup PIC interrupt registers, control heartbeat and PB inputs
bit debounce (unsigned char count);     //debounces buttons
        
#endif	/* INTERRUPT_H */

