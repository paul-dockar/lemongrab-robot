#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0xFF;                               //set all portA to input
    ADCON0 = 0b10000001;                        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;                        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//converts ADRESH and ADRESL into 1 short variable and returns this variable
unsigned short getAdc(void) {
    unsigned short adc_raw = 0;
    GO = 1;                                     //Starts ADC Conversion
    while(GO) {
        continue;
    }

    adc_raw = (ADRESH << 2) | (ADRESL >> 6);    //moves ADRESH and ADRESL to float
    return adc_raw;
}

//takes converted adc variable and converts into cm according to characterisation equation
unsigned short getAdcDist(unsigned short adc_raw) {
    short adc_distance_cm;
    if (adc_raw < 500) {                        //equation for 20-110cm range
        adc_distance_cm = 1/(((adc_raw)-18.67)/10930);
    }
    if (adc_raw >= 500) {                       //equation for <20cm range
        adc_distance_cm = 1/(((adc_raw)-352.5)/3210);
    }

    return adc_distance_cm;
}

//writes ADC distance in cm to LCD
void adcDisplay (void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(getAdcDist(getAdc()));
    lcdWriteString("cm");
}