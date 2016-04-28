#include "iRobot.h"

void setupIRobot(void) {
    ser_init();
    
    ser_putch(START);
    __delay_ms(5);
    ser_putch(FULL);
    __delay_ms(5);
}

void moveStraight(void) {
    total_distance_travel = 0;
    drive(DRIVE,195,200);
    
    while (total_distance_travel < 4000) {
        total_distance_travel += sensorDistance();
        lcdSetCursor(0x40);
        lcdWriteToDigitBCD (total_distance_travel);
       
        lcdWriteString("cm driven    ");
    }
    drive(DRIVE,0,0);
}
void moveSquare(void) {
    angle_turn = 0;
    total_distance_travel = 0;
    distance_travel = 0;
    
    int last_distance = 0;
    
    char i = 0;
    for (i; i!=4; i++) {
        drive(DRIVE,195,200);

        while (distance_travel <= 998) {
            distance_travel += sensorDistance();
            total_distance_travel = distance_travel + (last_distance);
            lcdSetCursor(0x40);
            lcdWriteToDigitBCD (total_distance_travel);
            lcdWriteString("cm driven    ");
        }
        drive(DRIVE,0,0);
        __delay_ms(800);
        drive(DRIVE,195,-200);

        while(angle_turn < 90) {
        angle_turn += angleDistance();
        }
        drive(DRIVE,0,0);
        __delay_ms(800);
        
        last_distance += distance_travel;
        distance_travel = 0;
        angle_turn = 0;
    }
}

void drive(char opscode, int right_wheel, int left_wheel) {
    char right_high = 0;
    char right_low = 0;
    char left_high = 0;
    char left_low = 0;
    
    right_high = (right_wheel >> 8);
    right_low = right_wheel;
    left_high = (left_wheel >> 8);
    left_low = left_wheel;

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

int displayBatteryCharge (void) {
    unsigned char high_byte, low_byte;			
    int final_byte;
	ser_putch(142); 							//get sensor data
	ser_putch(25); 
	high_byte = ser_getch();							//store sensor data to high
	low_byte = ser_getch();	
    
    final_byte = (high_byte << 8 | low_byte);
    
    return final_byte;
}

int displayBatteryCapacity (void) {
    unsigned char high_byte, low_byte;			
    int final_byte;
	ser_putch(142);
	ser_putch(26); 
	high_byte = ser_getch();
	low_byte = ser_getch();	
    
    final_byte = (high_byte << 8 | low_byte);
    
    return final_byte;
}

int displayBatteryVoltage (void) {
    unsigned char high_byte, low_byte;			
    int final_byte;
	ser_putch(142);
	ser_putch(22); 
	high_byte = ser_getch();
	low_byte = ser_getch();	
    
    final_byte = (high_byte << 8 | low_byte);
    
    return final_byte;
}