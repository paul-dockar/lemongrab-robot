/* 
 * File:   song.h
 * Author: Michael.Smith201
 *
 * Created on 20 May 2016, 2:37 PM
 */

#ifndef SONG_H
#define	SONG_H
#include "main.h"

void WriteSongToEeprom(void);
void songOne(unsigned char eeprom_address);
void songTwo(unsigned char eeprom_address);
void songThree(unsigned char eeprom_address);

#endif	/* SONG_H */

