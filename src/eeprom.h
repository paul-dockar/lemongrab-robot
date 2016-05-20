/*
 * File:   eeprom.h
 * Author: Michael.Smith201
 *
 * Created on 18 May 2016, 12:24 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "main.h"

void eeprom_write(unsigned char address, unsigned char value);
unsigned char eeprom_read(unsigned char address);

#endif	/* EEPROM_H */

