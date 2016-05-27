#include "eeprom.h"

void eepromWrite(unsigned char address, unsigned char value) {
	EEADR = address;                                            //selects the address to write to on EEPROM
    EEDATA = value;                                             //inputs the data value you want to write to the EEDATA register
    GIE = 0;                                                    //disables interrupts while the writing process is occurring
    WREN = 1;                                                   //sets write enable bit for EEPROM
    EECON2 = 0x55;                                              //write sequence
    EECON2 = 0xAA;
    WR = 1;
    GIE = 1;                                                    //enables the interrupts again
    WREN = 0;                                                   //clears write enable bit, wont effect current write
    while (WR) {                                                //wait until write is complete (WR is cleared by hardware once write is done)

    }
    EEIF = 0;                                                   //clear the write interrupt flag bit
}

unsigned char eepromRead(unsigned char address) {
    EEADR = address;                                            //selects the address to read
    EEPGD = 0;                                                  //indicate that you want to read from EEPROM memory
    RD = 1;                                                     //read
    unsigned char eeprom_data = EEDATA;                         //stores the value of the register into a variable
    return eeprom_data;
}
