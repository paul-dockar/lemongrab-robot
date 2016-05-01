#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);           //setup PIC registers for ADC to function
int adcDisplay(void);         //writes ADC distance in cm to LCD
void adcDisplayQuick(int distance);
int getAdc(void);              //converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdcDist(int adc_raw);   //takes converted adc variable and converts into cm according to characterisation equation

#endif	/* ADC_H */
