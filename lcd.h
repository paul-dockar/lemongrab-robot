/* 
 * File:   lcd.h
 * Author: Paul
 *
 * Created on 6 April 2016, 11:48 PM
 */

#ifndef LCD_H
#define	LCD_H

#include "main.h"

//defines
#define RS RE0			//LCD Register Select
#define RW RE1			//LCD Read Write
#define EN RE2			//LCD Enable
#define LCD_DATA PORTD	//LCD data pins are connected to Port D

//write controls to LCD
void lcdWriteControl(unsigned char databyte);

//write data to LCD
void lcd_WriteData(unsigned char databyte);

//move the LCD cursor to a particular location
void lcdSetCursor(unsigned char address);

void lcdWriteString(char * s);

//function accepts char between 0 and 999 and writes it to lcd display in 3 digits
void lcdWriteToDigitBCD(unsigned int data);

//function initalises the LCD module - check that ADCON1 setting doesn't conflict
void setupLCD(void);

#endif	/* LCD_H */

