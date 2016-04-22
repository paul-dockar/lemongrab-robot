#ifndef ADC_H
#define	ADC_H

#include "main.h"

unsigned short scan_360_ccw_step_count;
float new_adc_distance;
float closest_adc_distance;

void setupADC(void);                    //setup PIC ADC registers
void resetADC(void);
float getAdc(void);              //return ADC 10bit raw value
float getAdcDist(float adc_raw); //takes ADC raw and calculates to cm
void adcDisplay (void);                 //writes ADC raw and ADC distance to LCD
void findClosestWall(void);

#endif	/* ADC_H */
