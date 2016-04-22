#include "lcd.h"

//write controls to LCD
void lcdWriteControl(unsigned char databyte){
    EN = 0;
    RW = 0;
    RS = 0;
    LCD_DATA = databyte;
    EN = 1;
    EN = 0;
    __delay_ms(2);
}

//write data to LCD
void lcdWriteData(unsigned char databyte){
    EN = 0;
    RW = 0;
    RS = 1;
    LCD_DATA = databyte;
    EN = 1;
    EN = 0;
    __delay_ms(1);
}

//move the LCD cursor to a particular location
void lcdSetCursor(unsigned char address){
    address |= 0b10000000;		//format address command using mask
    lcdWriteControl(address);	//write address command
}

//write strings to LCD
void lcdWriteString(char * s){
    while(*s) lcdWriteData(*s++);
}

//function accepts char between 0 and 999 and writes it to lcd display in 3 digits
void lcdWriteToDigitBCD(unsigned int data){
    unsigned int OnesDigit;
    unsigned char TensDigit, HundredsDigit;

    //load number to be converted into OnesDigit
    OnesDigit = data;
    TensDigit = 0;
    HundredsDigit = 0;

    //Perform a BCD Conversion
    while (OnesDigit >= 100){
            OnesDigit = OnesDigit - 100;
            HundredsDigit++;
    }
    while (OnesDigit >= 10){
            OnesDigit = OnesDigit - 10;
            TensDigit++;
    }

    lcdWriteData(HundredsDigit + 48);
    lcdWriteData(TensDigit + 48);
    lcdWriteData(OnesDigit + 48);
}

//function initalises the LCD module - check that ADCON1 setting doesn't conflict
void setupLCD(void){
    //setup ADCON1 register to make PortE Digital
    PORTD = 0;				//set all pins on portd low
    PORTE = 0;				//set all pins on porte low

    TRISD = 0b00000000;		//set all pins to output
    TRISE = 0b00000000;		//set all pins to output

    //LCD Initialisation
    lcdWriteControl(0b00000001); //clear display
    lcdWriteControl(0b00111000); //set up display
    lcdWriteControl(0b00001100); //turn display on
    lcdWriteControl(0b00000110); //move to first digit
    lcdWriteControl(0b00000010); //entry mode setup

    lcdSetCursor(0X40);
    lcdWriteString("000 CCW");
}
