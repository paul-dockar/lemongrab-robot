#include "song.h"

void writeSongToEeprom(void) {
    songOne(EEPROM_ADDRESS_SONG_ONE);
    songTwo(EEPROM_ADDRESS_SONG_TWO);
    songThree(EEPROM_ADDRESS_SONG_THREE);
}

void songOne(unsigned char eeprom_address) {
    unsigned char song_one[] = {0, 11, 75, 32, 78, 16, 82, 16, 90, 16, 87, 48, 89, 32, 87, 16, 85, 16, 1, 16, 85, 16, 1, 32};

    for (char i = 0; i < SONG_ONE_SIZE; i++) {
        eepromWrite(eeprom_address, song_one[i]);
        eeprom_address++;
    }
}

void songTwo(unsigned char eeprom_address) {
    unsigned char song_two[] = {1, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32};

    for (char i = 0; i < SONG_TWO_SIZE; i++) {
        eepromWrite(eeprom_address, song_two[i]);
        eeprom_address++;
    }
}

void songThree(unsigned char eeprom_address) {
    unsigned char song_three[] = {2, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32};

    for (char i = 0; i < SONG_THREE_SIZE; i++) {
        eepromWrite(eeprom_address, song_three[i]);
        eeprom_address++;
    }
}