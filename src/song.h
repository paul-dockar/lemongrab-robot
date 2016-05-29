#ifndef SONG_H
#define	SONG_H
#include "main.h"

#define     EEPROM_ADDRESS_SONG_ONE     0x00
#define     EEPROM_ADDRESS_SONG_TWO     0x19
#define     EEPROM_ADDRESS_SONG_THREE   0x3C
#define     EEPROM_ADDRESS_SONG_FOUR    0x4D
#define     EEPROM_ADDRESS_SONG_FIVE    0x6C
#define     EEPROM_ADDRESS_SONG_SIX     0x8F
#define     EEPROM_ADDRESS_SONG_SEVEN   0x96
#define     SONG_ONE_SIZE               24
#define     SONG_TWO_SIZE               34
#define     SONG_THREE_SIZE             16
#define     SONG_FOUR_SIZE              30
#define     SONG_FIVE_SIZE              34
#define     SONG_SIX_SIZE               6
#define     SONG_SEVEN_SIZE             6

void writeSongToEeprom(void);
void songOne(unsigned char eeprom_address);
void songTwo(unsigned char eeprom_address);
void songThree(unsigned char eeprom_address);
void songFour(unsigned char eeprom_address);
void songFive(unsigned char eeprom_address);
void songSix(unsigned char eeprom_address);
void songSeven(unsigned char eeprom_address);

#endif	/* SONG_H */

