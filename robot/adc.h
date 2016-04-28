#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);           //setup PIC registers for ADC to function
void adcDisplay(void);         //writes ADC distance in cm to LCD
int getAdc(void);              //converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdcDist(int adc_raw);   //takes converted adc variable and converts into cm according to characterisation equation
int round(float x);            //rounds float to int

#endif	/* ADC_H */
