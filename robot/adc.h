#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);                    //setup PIC registers for ADC to function
int adcDisplay(void);                   //takes average of 4 ADRESH/ADRESL readings and writes ADC distance in cm to LCD
void adcDisplayQuick(int distance);     //writes ADC distance to LCD but without multiple readings or averages
int getAdc(void);                       //converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdcDist(int adc_raw);            //takes converted adc variable and converts into cm according to characterisation equation

#endif	/* ADC_H */
