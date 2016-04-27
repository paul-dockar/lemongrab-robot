#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);                                //setup PIC registers for ADC to function
unsigned short getAdc(void);                        //converts ADRESH and ADRESL into 1 short variable and returns this variable
unsigned short getAdcDist(unsigned short adc_raw);  //takes converted adc variable and converts into cm according to characterisation equation
void adcDisplay (void);                             //writes ADC distance in cm to LCD

#endif	/* ADC_H */
