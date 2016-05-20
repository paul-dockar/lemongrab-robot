#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0b00111111;         //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//takes average of 4 ADRESH/ADRESL readings and writes ADC distance in cm to LCD
int adcDisplayDistance(void) {
    int average_adc = 0;
    int adc_distance = 0;

    char i = 4;
    for (i; i!=0; i--) {
        average_adc += getAdc();
    }
    average_adc = average_adc/4;

    adc_distance = getAdcDist(average_adc);

    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(adc_distance);
    lcdWriteString("cm IR     ");

    return adc_distance;
}

//displays a distance value to the LCD. This function takes in the distance which is already captured during other functions, and does not average the value.
//Uses less time to write to LCD, which should not affect performance for manuevers that require precision.
void adcDisplayQuick(int distance) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(distance);
    lcdWriteString("cm IR     ");
}

//converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdc(void) {
    int adc_raw = 0;
    GO = 1;                                         //Starts ADC Conversion
    while(GO) continue;

    return adc_raw = (ADRESH << 2) | (ADRESL >> 6); //moves ADRESH and ADRESL to float
}

//takes converted adc variable and converts into cm according to characterisation equation
int getAdcDist(int adc_raw) {
    int adc_distance_cm;

    if (adc_raw > 500) {
        adc_distance_cm = 1/(((adc_raw)-376.1)/2520);
    } else {
        adc_distance_cm = 1/(((adc_raw)-7.5)/11907);
    }

    if (adc_distance_cm > 170 || adc_distance_cm < 0) return 170;
    else return adc_distance_cm;
}
