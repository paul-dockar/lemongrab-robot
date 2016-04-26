#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);
float getAdc(void);
float getAdcDist(float adc_raw);
void adcDisplay (void);

#endif	/* ADC_H */
