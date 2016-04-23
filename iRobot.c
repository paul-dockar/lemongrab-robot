#include "iRobot.h"

void setupIRobot(void){
    ser_putch(START); //start create robot
    __delay_ms(5);
    ser_putch(FULL);
    __delay_ms(5);
}
void drive(void){
    ser_putch(DRIVE); 
    __delay_ms(5); 
    ser_putch(0); 
    __delay_ms(5); 
    ser_putch(200); 
    __delay_ms(5); 
    ser_putch(200); 
    __delay_ms(5); 
    ser_putch(0);
    __delay_ms(5);
}

void stop(void){
    ser_putch(DRIVE); 
    __delay_ms(5); 
    ser_putch(0); 
    __delay_ms(5); 
    ser_putch(0); 
    __delay_ms(5); 
    ser_putch(0); 
    __delay_ms(5); 
    ser_putch(0);
    __delay_ms(5);
}

void figureEightTest(void) {
    ser_putch(136); 
    __delay_ms(5); 
    ser_putch(4); 
    __delay_ms(5); 
}
