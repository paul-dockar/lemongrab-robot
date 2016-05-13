#ifndef SPI_H
#define	SPI_H

#include "main.h"

#define     SM_Control_Byte SSPBUF                  //SM_Control_Byte is SSPBUF
#define     SELECT_NONE()	RC0 = 0; RC1 = 0;
#define     SELECT_EEPROM()	RC0 = 0; RC1 = 1;
#define     SELECT_RF()		RC0 = 1; RC1 = 0;
#define     SELECT_SM()		RC0 = 1; RC1 = 1;
#define     SM_STEP()		RC2 = 1; NOP(); RC2 = 0;

void setupSPI(void);                                //setup SPI module and port c input/output pins. sets stepper motor to be controlled from pic
unsigned char spi_transfer(unsigned char data);     //Transfers data through SPI using SSPBUF/SM_Control_Byte. Note used interchangeably here for illustration purposes

#endif	/* SPI_H */
