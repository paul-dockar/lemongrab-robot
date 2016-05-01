#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);           //setup PIC registers for ADC to function
void adcDisplay(void);         //writes ADC distance in cm to LCD
void adcDisplayQuick(unsigned int distance);
unsigned int getAdc(void);              //converts ADRESH and ADRESL into 1 int variable and returns this variable
unsigned int getAdcDist(unsigned int adc_raw);   //takes converted adc variable and converts into cm according to characterisation equation

#endif	/* ADC_H */
