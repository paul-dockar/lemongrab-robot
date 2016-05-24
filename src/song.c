#include "song.h"

/*void WriteSongToEeprom(void){
    songOne(EEPROM_ADDRESS_SONG_ONE);
    songTwo(EEPROM_ADDRESS_SONG_TWO);
    songThree(EEPROM_ADDRESS_SONG_THREE);
}

void songOne(unsigned char eeprom_address) {
    unsigned char song_one[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32};

    for (char i = 0; i < SONG_ONE_SIZE; i++) {
        eepromWrite(eeprom_address, song_one[i]);
        eeprom_address++;
    }
}

void songTwo(unsigned char eeprom_address) {
    unsigned char song_two[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32};

    for (char i = 0; i < SONG_TWO_SIZE; i++) {
        eepromWrite(eeprom_address, song_two[i]);
        eeprom_address++;
    }
}

void songThree(unsigned char eeprom_address) {
    unsigned char song_three[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32};

    for (char i = 0; i < SONG_THREE_SIZE; i++) {
        eepromWrite(eeprom_address, song_three[i]);
        eeprom_address++;
    }
}*/