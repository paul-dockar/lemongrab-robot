#include "adc.h"

unsigned short scan_360_ccw_step_count = 0;
float new_adc_distance = 0;

//setup PIC ADC registers
void setupADC(void) {
    TRISA = 0xFF;               //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue
    
    __delay_us(50);
}

void resetADC(void) {
    scan_360_ccw_step_count = 0;
    closest_adc_distance = getAdc();
}

//return ADC 10bit raw value
float getAdc(void) {
    float adc_raw = 0;
    GO = 1;                                 //Starts ADC Conversion
    while(GO) {
        continue;
    }
    
    adc_raw = (ADRESH << 2) | (ADRESL >> 6); //moves ADRESH and ADRESL to float
    return adc_raw;
}

//takes ADC raw and calculates to cm
float getAdcDist(float adc_raw) {
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
    lcdWriteToDigitBCD(getAdcDist(getAdc()));
    lcdWriteString("cm");
}

void findClosestWall(void) {
    new_adc_distance = getAdc();
    
    if (new_adc_distance > closest_adc_distance) {
        closest_adc_distance = new_adc_distance;
        scan_360_ccw_step_count = 0;
    }
    scan_360_ccw_step_count++;
}