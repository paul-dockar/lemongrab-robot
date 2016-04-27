#include "iRobot.h"

void setupIRobot(void){
    ser_init();
    
    ser_putch(START);
    __delay_ms(5);
    ser_putch(FULL);
    __delay_ms(5);
}
void drive(unsigned char opscode, unsigned char right_high, unsigned char right_low, unsigned char left_high, unsigned char left_low){
    ser_putch(opscode); 
    __delay_ms(5); 
    ser_putch(right_high); 
    __delay_ms(5); 
    ser_putch(right_low); 
    __delay_ms(5); 
    ser_putch(left_high); 
    __delay_ms(5); 
    ser_putch(left_low);
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
