/*
 * File:   song.h
 * Author: Michael.Smith201
 *
 * Created on 20 May 2016, 2:37 PM
 */

#ifndef SONG_H
#define	SONG_H
#include "main.h"

#define     EEPROM_ADDRESS_SONG_ONE     0x00
#define     EEPROM_ADDRESS_SONG_TWO     0x20
#define     EEPROM_ADDRESS_SONG_THREE   0x40
#define     SONG_ONE_SIZE               24
#define     SONG_TWO_SIZE               34
#define     SONG_THREE_SIZE             34

void writeSongToEeprom(void);
void songOne(unsigned char eeprom_address);
void songTwo(unsigned char eeprom_address);
void songThree(unsigned char eeprom_address);

#endif	/* SONG_H */

