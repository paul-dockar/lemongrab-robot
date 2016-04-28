#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0xFF;               //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//converts ADRESH and ADRESL into 1 int variable and returns this variable
unsigned int getAdc(void) {
    unsigned int adc_raw = 0;
    GO = 1;                                     //Starts ADC Conversion
    while(GO) continue;

    adc_raw = (ADRESH << 2) | (ADRESL >> 6);    //moves ADRESH and ADRESL to float
    return adc_raw;
}

//takes converted adc variable and converts into cm according to characterisation equation
unsigned int getAdcDist(unsigned int adc_raw) {
    unsigned int adc_distance_cm;
    if (adc_raw < 500) {
        adc_distance_cm = round(1/(((adc_raw)-18.67)/10930));  //equation for 20-110cm range
    }
    if (adc_raw >= 500) {
        adc_distance_cm = round(1/(((adc_raw)-352.5)/3210));   //equation for <20cm range
    }

    return adc_distance_cm;
}

//writes ADC distance in cm to LCD
void adcDisplay (void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(getAdcDist(getAdc()));
    lcdWriteString("cm");
}

//rounds float to int
unsigned int round (float x) {
    return (unsigned int)(x + 0.5f);
}

int sensordistance (void)
{
    volatile int highbyte, lowbyte;			
    volatile int finalbyte;
	ser_putch(142); 							//get sensor data
	ser_putch(19); 
	highbyte = ser_getch();							//store sensor data to high
	lowbyte = ser_getch();	
    
    finalbyte = (highbyte << 8 | lowbyte);
    
    return finalbyte;
   
}