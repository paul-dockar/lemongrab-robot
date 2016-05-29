#ifndef EEPROM_H
#define	EEPROM_H

#include "main.h"

void eepromWrite(unsigned char address, unsigned char value);
unsigned char eepromRead(unsigned char address);

#endif	/* EEPROM_H */

