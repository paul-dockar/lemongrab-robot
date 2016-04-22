#include "interrupt.h"

volatile unsigned int rtcCounter = 0;
volatile bit pbRelease = 0;
volatile unsigned char pbDebounceCount = 0;

 //setup PIC interrupt registers, control heartbeat and PB inputs
void interrupt isr(void) {
    if (T0IF) {
        T0IF = 0; 
        TMR0 = TMR0_VAL;
        rtcCounter++;
        
        if (rtcCounter % 500 == 0) {        //cycle LED at 1Hz
            LED0 = !LED0;
            rtcCounter = 0;
        }
        
        if (PBLED||PBCW||PBCCW||PBROT){                  //check for PB inputs
            pbDebounceCount++;
            if (debounce(pbDebounceCount) && PBLED) {   //call debounce & PB check
                pbLEDPressed = 1;                       //PB flag to ON
                pbRelease = 0;
            }
            if (debounce(pbDebounceCount) && PBCW) {
                pbCWPressed = 1;
                pbRelease = 0;
            }
            if (debounce(pbDebounceCount) && PBCCW) {
                pbCCWPressed = 1;
                pbRelease = 0;
            }
            if (debounce(pbDebounceCount) && PBROT) {
                pbROTPressed = 1;
                pbRelease = 0;
            }
        }
        else {
            pbDebounceCount = 0;
            pbRelease = 1;
        }
    }
}

//debounces buttons
bit debounce (unsigned char count) {
    if (count > DEBOUNCE_REQ_COUNT && pbRelease) {
        return 1;
    }
    else return 0;
}