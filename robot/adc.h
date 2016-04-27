#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);                            //setup PIC registers for ADC to function
unsigned int getAdc(void);                      //converts ADRESH and ADRESL into 1 int variable and returns this variable
unsigned int getAdcDist(unsigned int adc_raw);  //takes converted adc variable and converts into cm according to characterisation equation
void adcDisplay (void);                         //writes ADC distance in cm to LCD
unsigned int round (float x);                   //rounds float to int

#endif	/* ADC_H */
