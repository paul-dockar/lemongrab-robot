//----------------------------------------------------------------------------------------
//	Mechatronics 2 Integration Assignment 	
//	Group SBADJ
//
//	Saleh Aljimaz 		11549146
//	Bader Alqattan 		11550074
//	Adam Cheng 		11460779
//	Damon Croxton 		11379017
//	John Martin 		11394739
//
//----------------------------------------------------------------------------------------

XX----------------------------------------------------------------------------------------
XX	MAIN1.c FILE
XX----------------------------------------------------------------------------------------

#include "ser.h"
#include "lcd.h"
#include "adc.h"

#if (_HTC_VER_MAJOR_ < 9 && _HTC_VER_MINOR_ < 81)
__CONFIG(HS & WDTDIS & PWRTEN & BOREN & LVPDIS & DUNPROT & WRTEN & UNPROTECT);
#else
__CONFIG(FOSC_HS &  WDTE_OFF & CP_OFF & BOREN_OFF & PWRTE_ON & WRT_OFF & LVP_OFF & CPD_OFF);
#endif

//----------------------------------------------------------------------------------------
//	Definitions
//----------------------------------------------------------------------------------------

#define _XTAL_FREQ  		20000000					//Oscillator (Hz)
#define TMR0_SRT 		100						//Start Timer0 at 100
#define	SELECT_NONE()		RC0 = 0; RC1 = 0;				//SPI Motor control
#define	SELECT_SM()		RC0 = 1; RC1 = 1;
#define SM_STEP()		RC2 = 1; NOP(); RC2 = 0;

//Inputs (Buttons)
#define SCb !RB1								//360 Scan Button
#define Sb  !RB2								//Straight Mode Button
#define Lb  !RB3								//L turn Mode Button
#define Wb  !RB4								//Wall follow Mode Button
#define Bb  !RB5								//Turn motor counterclockwise button (unravels sensor cables)

//Outputs
#define	hLED RB0								//Heartbeat LED @1Hz

//Interrupt Variables
volatile unsigned int		hLEDCount = 0;					//Counts Timer0 overflows for heartbeat

//Global Variables
unsigned int 	distance = 0;							//For distance measuring
int 		angle = 0;							//For angle measuring
int 		test = 0;							//Holds ADC result during Wall-following scans
bit 		scandirection = 0;						//Indicates to alternate CW-CCW 360 degree scan
bit 		wallside = 0;							//Indicates which side of the robot the wall is on

void 		init(void);							//Prototype Functions
unsigned char 	spi_transfer(unsigned char data);

//----------------------------------------------------------------------------------------
//	Interrupt Routine
//----------------------------------------------------------------------------------------

void interrupt isr1() 
{
	ser_int();								//ensures serial operations happen correctly
	
	//Timer0 Buffer Overflow
	if(TMR0IF)								//TIMER0 Overflow Sequence
	{
		TMR0IF = 0;							//clear flag bit
		TMR0 = TMR0_SRT;						//Reset Timer0 back to 100 start count
		hLEDCount++;							//Increment overflow counter

		if(hLEDCount %500 == 0)						//If overflow counter = 500 (500ms)
		{
			hLEDCount = 0;						//Reset and toggle hLED
			hLED = !hLED;
		}
	}
}

//----------------------------------------------------------------------------------------
//	Initialise function
//----------------------------------------------------------------------------------------

void init()
{
	init_adc();								//Initialise ADC
	lcd_init();								//Initialise LCD
	ser_init(); 								//Initialise Serial Comm.
	
	TRISB = 0b11111110;							//Port B: all inputs for dipswitches with one output for heartbeat LED, Set at LOW
	PORTB = 0b00000000;							
	TRISC &= 0b10010000;							//set pin 6 to output for USART TX, pins for SPI and CS pins
	OPTION_REG = 0b00000100;						//Timer0 Setup (1:32 Prescalar, Interrupt on)

	SSPSTAT = 0b01000000;							//Initialise the SPI module
	SSPCON = 0b10100001; 
		
	TMR0IE = 1;								//Enable all interrupts
	PEIE = 1;	
	GIE  = 1;
	ei();
}

//----------------------------------------------------------------------------------------
//	SPI Transfer Function
//----------------------------------------------------------------------------------------
 
unsigned char spi_transfer(unsigned char data)					//SPI Transfer function to transfer data to/from CPLD
{
	unsigned char temp = 0;

	SSPIF = 0;
	SSPBUF = data;
	
	while (SSPIF == 0);  	
	temp = SSPBUF;
	SSPIF = 0;

	return temp;
}


//----------------------------------------------------------------------------------------
//	Drive Function for iRobot
//----------------------------------------------------------------------------------------

void drive(signed int vel, int c1, int c2)					//Convert selected velocity into two bytes of two's complement form, angle inputs as high/low byte
{
	signed int t1 = 0;							//Higher byte value for Velocity high byte
	if(vel<0)								//Sequence for negative velocity value.
	{
		t1 = 255;							
		while(vel<-256)							//Decriments the velocity high byte (starts at 255)
		{
			--t1;
			vel = vel + 255;
		}		
		vel = vel + 255;						//Corrects velocity low byte.
	}
	
	if(vel>=0)								//Sequence for positive velocity value.
	{
		while (vel>255)							//Incrememnts high byte if low byte > 255.
		{
			++t1;
			vel = vel - 256;
		}
	}
		
	ser_putch(137); 							//Push drive command over serial to irobot
	__delay_ms(100);
	ser_putch(t1); 								//Push velocity high byte
	__delay_ms(100);
	ser_putch(vel); 							//Push velocity low byte
	__delay_ms(100);
	ser_putch(c1); 								//Push angle high byte
	__delay_ms(100);
	ser_putch(c2); 								//Push angle low byte
	__delay_ms(100);
}

//----------------------------------------------------------------------------------------
//	Read sensor data from iRobot Functions   
//----------------------------------------------------------------------------------------

int get_distance()								//Returns 16 bit data from iRobot's distance sensor
{
	volatile int high, low;							//Declare varibles to hold high and low bytes from iRobot
	volatile int combined = 0;
	ser_putch(142); 							//Request distance travelled sensor data
	ser_putch(19); 
	high = ser_getch();							//Keep the high and low bytes
	low = ser_getch();
	combined = high << 8 | low;						//Bit shift the high byte and OR the low byte into 1 16 bit int

	return combined;							//Return the new combined 16 bit angle distance sensor reading
}


int get_angle()									//Returns 16 bit data from iRobot's angle sensor
{
	volatile int Ahigh, Alow;						//Follows the same process as above
	volatile int Acombined;
	ser_putch(142); 
	ser_putch(20); 
	Ahigh = ser_getch();
	Alow = ser_getch();
	Acombined = Ahigh << 8 | Alow;
	
	return (Acombined);
}

//----------------------------------------------------------------------------------------
//	Play a Tune Function through iRobot  (NOT USED IN FINAL DEMONSTRATION)
//----------------------------------------------------------------------------------------

void sound(unsigned int data)							//Plays the tune "data" number of times
{
	int i = 0;
	for(i=0;i<data;i++)
	{
		ser_putch(128); 						//Pushes commands to irobot to enter full mode
		__delay_ms(100);
		ser_putch(132); 
		__delay_ms(100);
		ser_putch(140); 						//Pushes the sound commands and notes the play to irobot
		__delay_ms(100);
		ser_putch(0); 
		__delay_ms(100);
		ser_putch(4); 
		__delay_ms(100);
		ser_putch(62); 
		__delay_ms(100);
		ser_putch(12); 
		__delay_ms(100);
		ser_putch(66); 
		__delay_ms(100);
		ser_putch(12); 
		__delay_ms(100);
		ser_putch(69); 
		__delay_ms(100);
		ser_putch(12); 
		__delay_ms(100);
		ser_putch(74); 
		__delay_ms(100);
		ser_putch(36); 
		__delay_ms(100);
		ser_putch(141); 
		__delay_ms(100);
		ser_putch(0); 
		__delay_ms(1500);
	}
	__delay_ms(1000);
}

//----------------------------------------------------------------------------------------
//	Stepper Motor Roatation Functions
//----------------------------------------------------------------------------------------

void fastCW(int steps)								//Rotate Clockwise Function
{
	SELECT_SM();								//Uses SPI Module to enable motor and turn clockwise
	spi_transfer(0b00001101);
	SELECT_NONE();
	__delay_ms(1);	
	
	for(int i=0;i<steps;i++)						//Steps the motor "steps" amount
	{
		SM_STEP();
		__delay_ms(10);
	}
	spi_transfer(0b00001100);						//De-energise motor
	SELECT_NONE();		
}	


void fastCCW(int steps)								//Rotate Counter Clockwise Function
{
	SELECT_SM();
	spi_transfer(0b00001111);						//Uses SPI Module to enable motor and turn counter-clockwise
	SELECT_NONE();
	__delay_ms(1);
	
	
	for(int i=0;i<steps;i++)						//Steps the motor "steps" amount
	{
		SM_STEP();
		__delay_ms(3);
	}
	spi_transfer(0b00001110);						//De-energise motor
	SELECT_NONE();		
}	

//----------------------------------------------------------------------------------------
//	360 Degree Scan and Point Function
//----------------------------------------------------------------------------------------

void Scan()									//Scans 360 deg and locates nearest point
{
	int largest = 0;							//Initialise variables (largest and position)
	int position = 0;
	lcd_write_control(0b00000001);						//Initialise screen (Clears)
	
	lcd_write_string("ADC Value: ");					//Displays onto LCD

	SELECT_SM();								//Uses SPI Module to enable motor and turn clockwise
	spi_transfer(0b00001101);
	SELECT_NONE();
	__delay_ms(20);	
	
	for(int i=0;i<400;i++)							//Loops 400 times (completes a full rev.)
	{
		SM_STEP();							//Motor Half step
		__delay_ms(4);
		if(adc_read_channel(0)>largest)					//If current reading > stored reading, overwrite (stores value + position)
		{
			largest = adc_read_channel(0);	
			position = i;
		}	

		lcd_write_6_digit_bcd(adc_read_channel(0));			//ADC value displayed onto LCD
	}
	
	lcd_write_control(0b00000001);						//After 360 scan, Clear and initialise LCD
	lcd_set_cursor(0x00);							
	lcd_write_string("Max ADC =");						//print the text and largest adc value obtained during the scan
	lcd_write_6_digit_bcd(largest);
	__delay_ms(400);

	lcd_set_cursor(0x40);							//Set LCD cursor to second row and Prints "Moving to Max"
	lcd_write_string("Moving to Max");

	__delay_ms(400);	
	fastCW(position);							//Step Motor to nearest location
	spi_transfer(0b00001100);						//De-energise motor
	SELECT_NONE();		
}

//----------------------------------------------------------------------------------------
//	Fast Alternating CW/CCW Scan Function (FOR WALL FOLLOWING)
//----------------------------------------------------------------------------------------

void Dscan()									//Continuously scans 360 degrees
{
	test = 0;								//Initialise test variable
	SELECT_SM();

	if(scandirection)spi_transfer(0b00001101);				//Checks the scan direction bit and initialises to step in either CW or CCW direction
	else spi_transfer(0b00001111);
	SELECT_NONE();
	__delay_ms(1);	
	
	for(int i=0;i<400;i++)							//Rotates motor in halfsteps for 360 deg.
	{
		SM_STEP();							
		__delay_ms(1);		
		if(adc_read_channel(0)>test)					//If ADC value > test variable, Overwrite
		{
			test = adc_read_channel(0);				
		}	
	}
	scandirection = !scandirection;						//Change Scan direction
	spi_transfer(0b00001100);						//De-energise Motor
	SELECT_NONE();
}

//----------------------------------------------------------------------------------------
//	Straight Mode Function
//----------------------------------------------------------------------------------------

void Straight()									//Drive for 2m, while displaying distance travelled
{
	distance = 0;								//Intialise local variable

	lcd_write_control(0b00000001);						//Initialise LCD to display "Dist Travelled: "
	lcd_set_cursor(0x00);
	lcd_write_string("Dist Travelled: ");

	__delay_ms(50); 
	drive(200,128,0);							//Drive Straight Command

	while(distance<1900)							//Continue till travelled 2000mm.
	{
		__delay_ms(20);
		distance += get_distance();					//Cumulative add distance from sensor
		lcd_set_cursor(0x40);
		lcd_write_6_digit_bcd(distance/10);				//Display distance in centimeters
		lcd_write_string(" cm"); 					
	}

  	drive(0,0,0);								//Stop iRobot
}

//----------------------------------------------------------------------------------------
//	L Turn Function Function
//----------------------------------------------------------------------------------------

void Lturn()									//L Turn Command (1m straight, Rotate 90 deg CW, 1m Straight)
{										
	angle = 0;								//Initialise local variables
	lcd_write_control(0b00000001);						//Initialise LCD and display "Distance Travelled: " 
	lcd_set_cursor(0x00);				
	lcd_write_string("Dist Travelled: ");

	drive(150,128,0);  							//Drive Straight Command

	while(distance<970)							//Loop while iRobot travels less than 1m
	{
		__delay_ms(20);
		distance += get_distance();					//Cumulative add distance data to total and Display onto LCD
		lcd_set_cursor(0x40);					
		lcd_write_6_digit_bcd(distance/10);				
		lcd_write_string(" cm");					

  		drive(0,0,0);							//Stop iRobot

		__delay_ms(100);					
	}
	
	drive(100,255,255);							//Turn iRobot Clockwise
	

	while(angle>=-69)							//Check if iRobot reaches 90 deg.
	{
		__delay_ms(50);							
		angle += get_angle();						//Cumilative add angle data to total and Display onto LCD
		lcd_set_cursor(0x4A);
		lcd_write_6_digit_bcd(angle);					
	}

	 drive(0,0,0);								//Stop iRobot turning.

	__delay_ms(300);

	drive(150,128,0);							//Drive Straight Command

	while(distance<1960)							//Loop while iRobot has traveled less than 2 total metres
	{
		__delay_ms(20);
		distance += get_distance();					//Cumulative add distance data to total and Display onto LCD
		lcd_set_cursor(0x40);					
		lcd_write_6_digit_bcd(distance/10);				
		lcd_write_string(" cm");					
	}
		
  	drive(0,0,0);  								//End Sequence (Stops iRobot)
}


//----------------------------------------------------------------------------------------
//	Wall Follow Function
//----------------------------------------------------------------------------------------

void Wallfollow()								//Wall Follow Sequence
{
	lcd_write_control(0b00000001);						//Initialise local variables and LCD
	test = 0;								
	int initial = 0;							
	int position = 0;							

	SELECT_SM();								//Use SPI Module to move Stepper Motor
	spi_transfer(0b00001101);
	SELECT_NONE();
	__delay_ms(20);	
	
	for(int i=0;i<400;i++)							//Scan 360 for closest location
	{
		SM_STEP();							
		if(adc_read_channel(0)>initial)					//if ADC value > largest, overwrite and store location
		{
			initial = adc_read_channel(0);		
			position = i;						
		}

		lcd_set_cursor(0x00);						//Display the adc value onto LCD
		lcd_write_6_digit_bcd(adc_read_channel(0));		
	}

	fastCCW(400);								//Untangle IR sensor cables (rotates motor in different direction)
	spi_transfer(0b00001100);						//De-energise Stepper motor
	SELECT_NONE();		
									
	lcd_set_cursor(0x00);						
	lcd_write_6_digit_bcd(initial);						//Display largest value onto LCD
	__delay_ms(200);

	if(position <= 200) wallside = 0;					//Determines which side the wall is located relative to the iRobot (Left or Right)
	if(position > 200) wallside = 1;					

	lcd_set_cursor(0x4B);	
	lcd_write_4_digit_bcd(wallside);					//Prints the wallside bit for testing purposes
	
	while(1)						
	{
		Dscan();							//Perform Alt. Scan function

		lcd_set_cursor(0x40);						//Prints value to LCD
		lcd_write_6_digit_bcd(test);						
	
		ser_putch(142);							//Determine if Wheel has dropped or been bumped, if so Stop iRobot
		ser_putch(7); 							
		if(ser_getch() > 0)						
		{								
				drive(0,0,0);
				return;
		}
		
		for(int i = 9;i<13;i++)						//Checks each of the 4 cliff sensors and stops iRobot if so.
		{
			ser_putch(142);						
			ser_putch(i);						//request packets 9 - 12
			if(ser_getch() > 0)					
			{		
				drive(0,0,0);					
				return;
			}
		}
							
		if(test < (initial + 15) && test > (initial - 15)) 		//if the test value from Dscan is within plus or minus 15 from the initial scan value
		{
			drive(30,128,0);					//drive straight forward
		}

		switch(wallside)						//enter a switch statement, utilising the wallside bit
		{
			case 0:							//case where the wall is on the right of the robot (wallside = 0)
			if(test > (initial + 15))				//if test is greater than (intial + 15)
			{
				drive(30,1,250);				//alter the drive of the robot with a radius turning it left, and away from the the wall
				__delay_ms(200);							
			}

			else if(test < (initial - 15))				//else if test is less than (intial - 15)
			{
				drive(30,-1,-250);				//alter the drive of the robot with a radius turning it right, towards the wall
				__delay_ms(200);
			}	

			break;							//break from the switch
			
			case 1:							//case where the wall is on the left side of the robot (wallside = 1)
			if(test > (initial + 15))				//if test is greater than (intial + 15)
			{
				drive(30,-1,-250);				//alter the drive of the robot with a radius turning it right, and away from the the wall
				__delay_ms(200);
			}			
	
			else if(test < (initial - 15))				//else if test is less than (intial - 15)
			{
				drive(30,1,250);				//alter the drive of the robot with a radius turning it left, towards the wall
				__delay_ms(200);		
			}

			break;	
		}		
	}			
}

//----------------------------------------------------------------------------------------
//	Main Function
//----------------------------------------------------------------------------------------

void main(void)
{
	init();									//Initialisation Function
	lcd_write_control(0b00000001);						//clear and set the cursor on the LCD
	lcd_set_cursor(0x00);							

	__delay_ms(3000);							
	ser_putch(128); 							//iRobot in Full Mode
	__delay_ms(100);	
	ser_putch(132); 	
	__delay_ms(100);
	
	lcd_write_string("Mech2Integration");					//TEST LCD DISPLAY
	lcd_set_cursor(0x43);		
	lcd_write_string("Team SBADJ");
	
	fastCW(8);								//Initialise the stepper motor through 8 steps
	SELECT_SM();								//De-energise the stepper motor
	spi_transfer(0b00001100);
	SELECT_NONE();
	
	while(1)								
	{
		SELECT_SM();							//De-energise Stepper Motor	
		spi_transfer(0b00001100);
		SELECT_NONE();
		
		//IF ANY BUTTON ARE SELECTED, PERFORM MANOEUVRE
		if(Lb)								//if Lb, perform L turn move
		{	
			Lturn();						
			while(Lb) __delay_ms(100);				//Checks if button is still on.
		}

		if(Sb)								//if Sb, perform straight move
		{
			Straight();						
			while(Sb) __delay_ms(100);				//Checks if button is still on.
		}

		if(SCb)								//if SCb, Scan nearest location
		{
			Scan();							
			while(SCb) __delay_ms(100);				//Checks if button is still on.
		}

		if(Wb)								//if Wb, perform wall follow
		{
			Wallfollow();						
			while(Wb) __delay_ms(100);				//Check if button is still on.
		}

		if(Bb)								//if Bb, Unravel IR sensor wire.
		{
			fastCCW(400);						
			while(Bb) __delay_ms(100);				//Check if button is still on.
		}
	}
}

XX----------------------------------------------------------------------------------------
XX	ADC.C FILE
XX----------------------------------------------------------------------------------------

#include <htc.h>
#include "adc.h"
#include "ser.h"
#define _XTAL_FREQ 20000000 

unsigned int adc_read_channel(unsigned char channel)		//finds correct values for adc channel
{
	switch(channel)											//Switch gives the correct values for each possible channel
	{
		case 0: 
				CHS0 = 0;
				CHS1 = 0;
				CHS2 = 0;
				break;
		case 1: 
				CHS0 = 1;
				CHS1 = 0;
				CHS2 = 0;
				break;
		case 2: 
				CHS0 = 0;
				CHS1 = 1;
				CHS2 = 0;
				break;
		case 3: 
				CHS0 = 1;
				CHS1 = 1;
				CHS2 = 0;
				break;
		case 4: 
				CHS0 = 0;
				CHS1 = 0;
				CHS2 = 1;
				break;


		default: 
				return 0;
	}

	__delay_us(50);		
	
	return adc_read();

}


void init_adc(void)					//initialise the adc port and registers
{
	//set ports
	PORTA = 0;
	TRISA = 0b00111111;				//all inputs

							//set control registers
	ADCON0 = 0b10100001;				//Fosc/32, Channel 4, ADC on
	ADCON1 = 0b00000010;				//left justified, PortE Digital, PortA Analogue

	__delay_us(50);					// Delay for ADC aquisition

}


											
unsigned int adc_read(void)				//get an adc result from the selected channel
{
	volatile unsigned int adc_value = 0;

	ADRESH	=	0; 	 			// Reset the ADRES value register 

	GO = 1;				  		// Staring the ADC process 					
  	while(GO) continue;			   	// Wait for conversion complete 			
//	asm("nop");
//	asm("nop");
//	asm("nop");		
		

	adc_value	=	 ADRESH;		//sets the adc value from the high and low registers
	adc_value <<=2;
	adc_value |= ADRESL>>6;


 	return (adc_value);     			// Return the value of the ADC process
}

XX----------------------------------------------------------------------------------------
XX	LCD.C FILE
XX----------------------------------------------------------------------------------------

#include <htc.h>
#include "lcd.h"
#include "ser.h"

#define _XTAL_FREQ 20000000
//functions
//write controls to LCD
void lcd_write_control(unsigned char databyte)
{
	EN = 0;
	RW = 0;
	RS = 0;
	LCD_DATA = databyte;
	EN = 1;
	EN = 0;
	__delay_ms(2);
}

//write data to LCD
void lcd_write_data(unsigned char databyte)
{
	EN = 0;
	RW = 0;
	RS = 1;
	LCD_DATA = databyte;
	EN = 1;
	EN = 0;
	__delay_ms(1);
}

//move the LCD cursor to a particular location
void lcd_set_cursor(unsigned char address)
{
	address |= 0b10000000;				//format address command using mask
	lcd_write_control(address);			//write address command
}

void lcd_write_string(char * s)
{
	// write characters
	while(*s) lcd_write_data(*s++);
}

//Writes a 4 digit input into LCD
void lcd_write_4_digit_bcd( int data)
{
	unsigned int OnesDigit, TensDigit, HundDigit, ThouDigit;

	//Store data into OnesDigit, and clears TensDigit and HundDigit
	OnesDigit = data;
	TensDigit = 0;
	HundDigit = 0;
	ThouDigit = 0;

	//Perform a BCD Conversion	
	while (OnesDigit >= 1000)
	{
		OnesDigit = OnesDigit - 1000;
		ThouDigit++;
	}	
	while (OnesDigit >= 100)
	{
		OnesDigit = OnesDigit - 100;
		HundDigit++;
	}	
	while (OnesDigit >= 10)
	{
		OnesDigit = OnesDigit - 10;
		TensDigit++;
	}
	lcd_write_data((unsigned int)ThouDigit + 48);
	lcd_write_data((unsigned int)HundDigit + 48);		
	lcd_write_data((unsigned int)TensDigit + 48);
	lcd_write_data((unsigned int)OnesDigit + 48);
}


void lcd_write_6_digit_bcd(unsigned int data)
{
	unsigned int OnesDigit, TensDigit, HundDigit, ThouDigit, TentDigit, HuntDigit;

	//Store data into OnesDigit, and clears TensDigit, HundDigit, ThouDigit, TentDigit, HuntDigit
	OnesDigit = data;
	TensDigit = 0;
	HundDigit = 0;
	ThouDigit = 0;
	TentDigit = 0;
	HuntDigit = 0;
	
	//Perform a BCD Conversion
	while (OnesDigit >= 100000)
	{
		OnesDigit = OnesDigit - 100000;
		HuntDigit++;
	}		
	while (OnesDigit >= 10000)
	{
		OnesDigit = OnesDigit - 10000;
		TentDigit++;
	}		
	while (OnesDigit >= 1000)
	{
		OnesDigit = OnesDigit - 1000;
		ThouDigit++;
	}	
	while (OnesDigit >= 100)
	{
		OnesDigit = OnesDigit - 100;
		HundDigit++;
	}	
	while (OnesDigit >= 10)
	{
		OnesDigit = OnesDigit - 10;
		TensDigit++;
	}
	lcd_write_data((unsigned int)HuntDigit + 48);
	lcd_write_data((unsigned int)TentDigit + 48);
	lcd_write_data((unsigned int)ThouDigit + 48);
	lcd_write_data((unsigned int)HundDigit + 48);		
	lcd_write_data((unsigned int)TensDigit + 48);
	lcd_write_data((unsigned int)OnesDigit + 48);
}

//function initalises the LCD module - check that ADCON1 setting doesn't conflict
void lcd_init(void)
{
	//setup ADCON1 register to make PortE Digital
	ADCON1 = 0b00000010;	//left justified, PortE Digital, PortA Analogue

	PORTD = 0;			//set all pins on portd low
	PORTE = 0;			//set all pins on porte low

	TRISD = 0b00000000;		//set all pins to output
	TRISE = 0b00000000;		//set all pins to output

	//LCD Initialisation
	lcd_write_control(0b00000001); //clear display
	lcd_write_control(0b00111000); //set up display
	lcd_write_control(0b00001100); //turn display on
	lcd_write_control(0b00000110); //move to first digit
	lcd_write_control(0b00000010); //entry mode setup

}

XX----------------------------------------------------------------------------------------
XX	SER.C FILE
XX----------------------------------------------------------------------------------------

/*
 * Distributed by www.microchipC.com - one of the web's largest
 * repositories of C source code for Microchip PIC micros.
 *
 * ser.c
 *
 * Interrupt Driven Serial Routines with Circular FIFO
 * Copyright (c) 2006, Regulus Berdin
 * All rights reserved. 
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL REGULUS BERDIN BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
*/

#define _SER_C_
#include <htc.h>
#include "ser.h"

unsigned char rxfifo[SER_BUFFER_SIZE];
volatile unsigned char rxiptr, rxoptr;
bank1 unsigned char txfifo[SER_BUFFER_SIZE];
volatile unsigned char txiptr, txoptr;
unsigned char ser_tmp;

bit ser_isrx(void)
{
	if(OERR) {
		CREN = 0;
		CREN = 1;
		return 0;
	}
	return (rxiptr!=rxoptr);
}

unsigned char ser_getch(void)
{
	unsigned char c;

	while (ser_isrx()==0)
		continue;

	GIE=0;
	c=rxfifo[rxoptr];
	++rxoptr;
	rxoptr &= SER_FIFO_MASK;
	GIE=1;
	return c;
}

unsigned char getch(){
	while(!RCIF);
	unsigned char rxbyte = RCREG;
	return rxbyte;		
}



void ser_putch(unsigned char c)
{
	while (((txiptr+1) & SER_FIFO_MASK)==txoptr)
		continue;
	GIE=0;
	txfifo[txiptr] = c;
	txiptr=(txiptr+1) & SER_FIFO_MASK;
	TXIE=1;
	GIE=1;
}

void ser_puts(const unsigned char * s)
{
	while(*s)
		ser_putch(*s++);
}

void ser_puts2(unsigned char * s)
{
	while(*s)
		ser_putch(*s++);
}

void ser_puthex(unsigned char v)
{
	unsigned char c;

	c = v >> 4;
	if (c>9) {
		ser_putch('A'-10+c);
	} else {
		ser_putch('0'+c);
	}

	c = v & 0x0F;
	if (c>9) {
		ser_putch('A'-10+c);
	} else {
		ser_putch('0'+c);
	}
}


void ser_init(void)
{
	TRISC |= 0b10000000;			//set RC7 to input for RX
	TRISC &= 0b10111111;			//set RC6 to output for TX
	BRGH=1;					//high speed

	SPBRG=20;				//57.6K @ 20MHz, SPBRG = (20MHz/(16*BAUD_RATE))-1;
//	SPBRG=10;				//115.2K @ 20MHz, SPBRG = (20MHz/(16*BAUD_RATE))-1;

	TX9=0;					//8 bits
	RX9=0;					//

	SYNC=0;					//uart settings
	SPEN=1;
	CREN=1;
	TXIE=0;
	RCIE=1;
	TXEN=1;
	//PEIE=1;		//enable peripheral interrupts and global interrupts in main function

	rxiptr=rxoptr=txiptr=txoptr=0;
}

XX----------------------------------------------------------------------------------------
XX	ADC.H FILE
XX----------------------------------------------------------------------------------------

#ifndef	ADC_H
#define ADC_H

#include <htc.h>

void init_adc(void);
unsigned int adc_read(void);
unsigned int adc_read_channel(unsigned char channel);
unsigned int Find_Distance(unsigned int ADCvalue);

#endif

XX----------------------------------------------------------------------------------------
XX	LCD.H FILE
XX----------------------------------------------------------------------------------------

#ifndef	LCD_H
#define LCD_H

#include <htc.h>


//defines
#define RS RE0			//LCD Register Select
#define RW RE1			//LCD Read Write
#define EN RE2			//LCD Enable
#define LCD_DATA PORTD	//LCD data pins are connected to Port D

#define _XTAL_FREQ 20000000

//LCD Operations~~~~~~~~~~~~~
//Write controls to LCD
void lcd_write_control(unsigned char databyte);

//Write data to LCD
void lcd_write_data(unsigned char databyte);

//Move the LCD cursor to a particular location
void lcd_set_cursor(unsigned char address);

//Writes a string onto the LCD
void lcd_write_string(char * s);

//Writes a 4 digit input into LCD
void lcd_write_4_digit_bcd( int data);

//Writes a 6 digit input into LCD
void lcd_write_6_digit_bcd(unsigned int data);

//Initalises the LCD module
void lcd_init(void);

#endif //LCD_H

XX----------------------------------------------------------------------------------------
XX	SER.H FILE
XX----------------------------------------------------------------------------------------

/*
 * ser.h
 *
 * Interrupt Driven Serial Routines with Circular FIFO
 * Copyright (c) 2006, Regulus Berdin
 * All rights reserved. 
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL REGULUS BERDIN BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef _SER_H_
#define _SER_H_

/* Valid buffer size value are only power of 2 (ex: 2,4,..,64,128) */
#define SER_BUFFER_SIZE		16
		
#define SER_FIFO_MASK 		(SER_BUFFER_SIZE-1)

/* Insert this macro inside the interrupt routine */
#define ser_int() 							\
	if (RCIF) {								\
		rxfifo[rxiptr]=RCREG;				\
		ser_tmp=(rxiptr+1) & SER_FIFO_MASK;	\
		if (ser_tmp!=rxoptr)				\
			rxiptr=ser_tmp;					\
	}										\
	if (TXIF && TXIE) {						\
		TXREG = txfifo[txoptr];				\
		++txoptr;							\
		txoptr &= SER_FIFO_MASK;			\
		if (txoptr==txiptr) {				\
			TXIE = 0;						\
		}									\
	}

//get byte from serial port
unsigned char getch(void);

bit ser_isrx(void);
unsigned char ser_getch(void);
void ser_putch(unsigned char byte);
void ser_puts(const char * s);
void ser_puts2(unsigned char * s);
void ser_puthex(unsigned char v);
void ser_init(void);

#ifndef _SER_C_
extern unsigned char rxfifo[SER_BUFFER_SIZE];
extern volatile unsigned char rxiptr, rxoptr;
extern bank1 unsigned char txfifo[SER_BUFFER_SIZE];
extern volatile unsigned char txiptr, txoptr;
extern unsigned char ser_tmp;
#endif

#endif


											