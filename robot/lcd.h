#ifndef LCD_H
#define	LCD_H

#include "main.h"

#define RS          RE0     //LCD Register Select
#define RW          RE1     //LCD Read Write
#define EN          RE2     //LCD Enable
#define LCD_DATA    PORTD   //LCD data pins are connected to Port D

void setupLCD(void);                             //function initalises the LCD module - check that ADCON1 setting doesn't conflict. Displays battery condition on startup
void lcdWriteControl(unsigned char databyte);    //write controls to LCD
void lcdWriteData(unsigned char databyte);      //write data to LCD
void lcdSetCursor(unsigned char address);        //move the LCD cursor to a particular location
void lcdWriteString(char * s);                   //write strings to LCD
void lcdWriteToDigitBCD(unsigned int data);      //function accepts char between 0 and 9999 and writes it to lcd display in seperate 4 digits
void lcdWriteToDigitBCDx6(unsigned int data);    //function accepts char between 0 and 999999 and writes it to lcd display in seperate 6 digits

#endif	/* LCD_H */
