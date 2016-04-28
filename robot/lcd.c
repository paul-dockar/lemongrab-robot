#include "lcd.h"

//function initalises the LCD module - check that ADCON1 setting doesn't conflict
void setupLCD(void) {
    //setup ADCON1 register to make PortE Digital
    PORTD = 0;				        //set all pins on portd low
    PORTE = 0;				        //set all pins on porte low

    TRISD = 0b00000000;		        //set all pins to output
    TRISE = 0b00000000;		        //set all pins to output

    //LCD Initialisation
    lcdWriteControl(0b00000001);    //clear display
    lcdWriteControl(0b00111000);    //set up display
    lcdWriteControl(0b00001100);    //turn display on
    lcdWriteControl(0b00000110);    //move to first digit
    lcdWriteControl(0b00000010);    //entry mode setup
    lcdWriteControl(0b00000001);    //clear display
    
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(displayBatteryCharge());
    lcdWriteString("/");
    lcdWriteToDigitBCD(displayBatteryCapacity());
    lcdWriteString("mAh");
    
    lcdSetCursor(0x40);
    lcdWriteToDigitBCDx6(displayBatteryVoltage());
    lcdWriteString("mV");
    __delay_ms(8000);
    lcdWriteControl(0b00000001);    //clear display
    
    lcdSetCursor(0x40);
    lcdWriteString("0cm driven      ");
}

//write controls to LCD
void lcdWriteControl(unsigned char databyte) {
    EN = 0;
    RW = 0;
    RS = 0;
    LCD_DATA = databyte;
    EN = 1;
    EN = 0;
    __delay_ms(2);
}

//write data to LCD
void lcdWriteData(unsigned char databyte) {
    EN = 0;
    RW = 0;
    RS = 1;
    LCD_DATA = databyte;
    EN = 1;
    EN = 0;
    __delay_ms(1);
}

//move the LCD cursor to a particular location
void lcdSetCursor(unsigned char address) {
    address |= 0b10000000;		    //format address command using mask
    lcdWriteControl(address);	    //write address command
}

//write strings to LCD
void lcdWriteString(char * s) {
    while(*s) lcdWriteData(*s++);
}

//function accepts char between 0 and 999 and writes it to lcd display in seperate 3 digits
void lcdWriteToDigitBCD(unsigned int data) {
    unsigned int    ones_digit;
    unsigned char   tens_digit, hundreds_digit, thousands_digit;

    //load number to be converted into OnesDigit
    ones_digit = data;
    tens_digit = 0;
    hundreds_digit = 0;
    thousands_digit = 0;

    //Perform a BCD Conversion
    while (ones_digit >= 1000){
        ones_digit = ones_digit - 1000;
        thousands_digit++;
    }
    while (ones_digit >= 100){
        ones_digit = ones_digit - 100;
        hundreds_digit++;
    }
    while (ones_digit >= 10){
        ones_digit = ones_digit - 10;
        tens_digit++;
    }
   
    if (data >= 1000) lcdWriteData(thousands_digit + 48);
    if (data >= 100)  lcdWriteData(hundreds_digit + 48);
    if (data >= 10)   lcdWriteData(tens_digit + 48);
    if (data >= 0)    lcdWriteData(ones_digit + 48);
}

void lcdWriteToDigitBCDx6(unsigned int data) {
    unsigned int    ones_digit;
    unsigned char   tens_digit, hundreds_digit, thousands_digit, ten_thousands_digit, hund_thousands_digit;

    //load number to be converted into OnesDigit
    ones_digit = data;
    tens_digit = 0;
    hundreds_digit = 0;
    thousands_digit = 0;
    ten_thousands_digit = 0;
    hund_thousands_digit = 0;

    //Perform a BCD Conversion
    while (ones_digit >= 100000){
        ones_digit = ones_digit - 100000;
        hund_thousands_digit++;
    }
    while (ones_digit >= 10000){
        ones_digit = ones_digit - 10000;
        ten_thousands_digit++;
    }
    while (ones_digit >= 1000){
        ones_digit = ones_digit - 1000;
        thousands_digit++;
    }
    while (ones_digit >= 100){
        ones_digit = ones_digit - 100;
        hundreds_digit++;
    }
    while (ones_digit >= 10){
        ones_digit = ones_digit - 10;
        tens_digit++;
    }
   
    
    lcdWriteData(hund_thousands_digit + 48);
    lcdWriteData(ten_thousands_digit + 48);
    lcdWriteData(thousands_digit + 48);
    lcdWriteData(hundreds_digit + 48);
    lcdWriteData(tens_digit + 48);
    lcdWriteData(ones_digit + 48);
}
