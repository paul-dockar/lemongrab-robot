#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0xFF;               //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdc(void) {
    unsigned int adc_raw = 0;
    GO = 1;                                         //Starts ADC Conversion
    while(GO) continue;

    return adc_raw = (ADRESH << 2) | (ADRESL >> 6); //moves ADRESH and ADRESL to float
}

//takes converted adc variable and converts into cm according to characterisation equation
int getAdcDist(int adc_raw) {
    int adc_distance_cm;
    
    if (adc_raw > 500) {
        adc_distance_cm = round(1/(((adc_raw)-376)/2520));
    }
    if (adc_raw <= 500) {
        adc_distance_cm = round(1/(((adc_raw)-7.5403)/11907));
    }

    return adc_distance_cm;
}

//writes ADC distance in cm to LCD
void adcDisplay (void) {
    int average_adc = 0;
    
    char i = 4;
    for (i; i!=0; i--) {
        average_adc += getAdc();
    }
    average_adc = average_adc/4;
    
    lcdSetCursor(0x00);
    if (average_adc > 80) lcdWriteToDigitBCD(getAdcDist(average_adc));
    else lcdWriteString(">160");
    
    lcdWriteString("cm IR    ");
}

//rounds float to int
int round (float x) {
    return (int)(x + 0.5f);
}

int sensorDistance (void){
    char high_byte, low_byte;			
    int final_byte;
	ser_putch(142); 							//get sensor data
	ser_putch(19); 
	high_byte = ser_getch();							//store sensor data to high
	low_byte = ser_getch();	
    
    final_byte = (high_byte << 8 | low_byte);
    
    __delay_ms(15);
    
    return final_byte;
}

int angleDistance (void){
    char high_angle, low_angle;
    int final_angle;
    ser_putch(142);
    ser_putch(20);
    high_angle = ser_getch();
    low_angle = ser_getch();
    
    final_angle = (high_angle << 8 | low_angle);
    
    __delay_ms(15);
    
    return final_angle;
}