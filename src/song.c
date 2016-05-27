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
    unsigned char song_two[] = {1, 16, 97, 8, 95, 8, 97, 32, 90, 32, 1, 48, 98, 8, 97, 8, 98, 16, 97, 16, 95, 48, 1, 32, 98, 8, 97, 8, 98, 32, 90, 32, 1, 128};

    for (char i = 0; i < SONG_TWO_SIZE; i++) {
        eepromWrite(eeprom_address, song_two[i]);
        eeprom_address++;
    }
}

void songThree(unsigned char eeprom_address) {
    unsigned char song_three[] = {2, 7, 95, 8, 93, 8, 95, 16, 93, 16, 92, 16, 95, 16, 93, 48};

    for (char i = 0; i < SONG_THREE_SIZE; i++) {
        eepromWrite(eeprom_address, song_three[i]);
        eeprom_address++;
    }
}