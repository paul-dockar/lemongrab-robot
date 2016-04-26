/* 
 * File:   spi.h
 * Author: Paul
 *
 * Created on 26 April 2016, 5:13 PM
 */

#ifndef SPI_H
#define	SPI_H

#include "main.h"

//Defines
//Osillator (Hz)
#define _XTAL_FREQ 20000000

#define SM_Control_Byte SSPBUF	//SM_Control_Byte is SSPBUF

//Macros that you can call for ease of use
#define     SELECT_NONE()	RC0 = 0; RC1 = 0;
#define     SELECT_EEPROM()	RC0 = 0; RC1 = 1;
#define     SELECT_RF()		RC0 = 1; RC1 = 0;
#define     SELECT_SM()		RC0 = 1; RC1 = 1;
#define     SM_STEP()		RC2 = 1; NOP(); RC2 = 0;

//Function prototypes
void setupSPI(void);
unsigned char spi_transfer(unsigned char data);

#endif	/* SPI_H */

