#include "iRobot.h"

void setupIRobot(void) {
    ser_init();
    
    ser_putch(START);
    __delay_ms(5);
    ser_putch(FULL);
    __delay_ms(5);
}

void moveStraight(void) {
    distance_travel = 0;
    drive(DRIVE, 0, 200, 0, 200);
    
    while (distance_travel < 4000) {
        distance_travel += sensorDistance();
        lcdSetCursor(0x40);
        lcdWriteToDigitBCD (distance_travel);
       
        lcdWriteString("cm driven   ");
    }
    drive(DRIVE,0,0,0,0);
}
void moveSquare(void) {
    angle_turn = 0;
    distance_travel = 0;
    
//    char i = 4;
//    for (i; i!=0; i--) {
//        
//    }

    drive(DRIVE, 0, 200, 0, 200);
    
    while (distance_travel <= 1000) {
        distance_travel += sensorDistance();
        lcdSetCursor(0x40);
        lcdWriteToDigitBCD (distance_travel);
       
        lcdWriteString("cm driven   ");
    }
    drive(DRIVE,0,0,0,0);
    __delay_ms(200);
    drive(DRIVE,0,200,0,-200);

    while(angle_turn < 90) {
    angle_turn += angleDistance();
    }
    drive(DRIVE,0,0,0,0);

}

void drive(char opscode, char right_high, char right_low, char left_high, char left_low) {
    //change this function to take in just right and high ints, convert to high low chars
    //adc_raw = (ADRESH << 2) | (ADRESL >> 6)
//    int temp = right_wheel;
//    
//    right_high = (right_wheel << 8)
//    
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