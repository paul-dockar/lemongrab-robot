#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0b00111111;         //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//takes average of 4 ADRESH/ADRESL readings and writes ADC distance in cm to LCD
void adcDisplay(void) {
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

void adcDisplayQuick(int distance) {    
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(distance);
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

//rounds float to int
int round(float x) {
    return (int)(x + 0.5f);
}
