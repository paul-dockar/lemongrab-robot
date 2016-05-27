/*
 * File:   eeprom.h
 * Author: Michael.Smith201
 *
 * Created on 18 May 2016, 12:24 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "main.h"

void eepromWrite(unsigned char address, unsigned char value);
unsigned char eepromRead(unsigned char address);

#endif	/* EEPROM_H */

