/* 
 * File:   adc.h
 * Author: Paul
 *
 * Created on 6 April 2016, 11:47 PM
 */

#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);                //setup PIC ADC registers
unsigned int getADC(void);          //return ADC 10bit raw value
float getADCdist(unsigned int raw); //takes ADC raw and calculates to cm
void adcDisplay (void);             //writes ADC raw and ADC distance to LCD

#endif	/* ADC_H */

