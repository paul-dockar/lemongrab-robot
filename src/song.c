#include "song.h"

void WriteSongToEeprom(void){
    unsigned char eeprom_address;
    
    //write song one
    eeprom_address = 0x00;
    songOne(eeprom_address);
    
    //write song two
    eeprom_address = 0x20;
    songTwo(eeprom_address);
    
    //write song three
    eeprom_address = 0x40;
    songThree(eeprom_address);   
}

void songOne(unsigned char eeprom_address) {
    unsigned char song_one[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32,};
    
    for (char i = 0; i <34; i++) {
        eeprom_write(eeprom_address, song_one[i]);
        eeprom_address++;
    }   
}

void songTwo(unsigned char eeprom_address) {
    unsigned char song_two[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32,};
    
    for (char i = 0; i <34; i++) {
        eeprom_write(eeprom_address, song_two[i]);
        eeprom_address++;
    }   
}

void songThree(unsigned char eeprom_address) {
    unsigned char song_three[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32,};
    
    for (char i = 0; i <34; i++) {
        eeprom_write(eeprom_address, song_three[i]);
        eeprom_address++;
    }   
}