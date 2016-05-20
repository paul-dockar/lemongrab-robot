#include "song.h"


void WriteSongToEeprom(void){
    unsigned char *eeprom_address;
    eeprom_address = 0x00;
    songOne(eeprom_address);
    
   
    
}

void songOne(unsigned char *eeprom_address) {
    unsigned char song_one[] = {0, 16, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32, 72, 32,};
    
    for (char i = 0; i <34; i++) {
        eeprom_write(eeprom_address, song_one[i]);
        eeprom_address++;
    }
    
    
    
}