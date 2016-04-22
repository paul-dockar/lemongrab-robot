#include "adc.h"

//setup PIC ADC registers
void setupADC(void) {
    TRISA = 0xFF;               //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue
    
    __delay_us(50);
}

//return ADC 10bit raw value
unsigned int getADC(void) {
    unsigned int adc_raw = 0;
    GO = 1;                                 //Starts ADC Conversion
    while(GO) {
        continue;
    }
    
    adc_raw = (ADRESH << 2) | (ADRESL >> 6); //moves ADRESH and ADRESL to int
    return adc_raw;
}

//takes ADC raw and calculates to cm
float getADCdist(unsigned int adc_raw) {
    float adc_distance_cm;
    if (adc_raw < 500) {                //equation for 20-110cm range
        adc_distance_cm = 1/(((adc_raw)-18.67)/10930);
    }
    if (adc_raw >= 500) {               //equation for <20cm range
        adc_distance_cm = 1/(((adc_raw)-352.5)/3210);
    }
    
    return adc_distance_cm;
}

//writes ADC distance to LCD
void adcDisplay (void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(getADCdist(getADC()));
    lcdWriteString("cm");
}