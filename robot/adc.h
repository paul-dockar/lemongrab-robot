#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);
unsigned short getAdc(void);
unsigned short getAdcDist(unsigned short adc_raw);
void adcDisplay (void);

#endif	/* ADC_H */
