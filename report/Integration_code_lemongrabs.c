/*
 * Team:    Lemongrabs
 * Authors: Paul Dockar     - 12059869
 *          Dennis Kim      - 12050466
 *          Kim Izzo        - 11971278
 *          Nick Rabey      - 11988589
 *          Michael Smith   - 11745167
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#pragma config BOREN = OFF, CPD = OFF, WRT = OFF, FOSC = HS, WDTE = OFF, CP = OFF, LVP = OFF, PWRTE = OFF
#include "adc.h"
#include "interrupt.h"
#include "iRobot.h"
#include "lcd.h"
#include "ser.h"
#include "spi.h"
#include "stepper.h"

#define _XTAL_FREQ      20000000        //oscillator (Hz)
#define HB_LED          RB0             //set PortB Pin 0 for heartbeat LED
#define PB_SCAN         !RB1            //set PortB Pin 1 for pushbutton scan closest wall
#define PB_DRIVE_4M     !RB2            //set PortB Pin 2 for pushbutton drive 4m straight line
#define PB_DRIVE_SQUARE !RB4            //set PortB Pin 3 for pushbutton drive 1m square
#define PB_FIND_WALL    !RB5            //set PortB Pin 4 for pushbutton drive along closest wall

volatile bit pb_scan_pressed;           //push button flag for PB_SCAN
volatile bit pb_drive_4m_pressed;       //push button flag for PB_DRIVE_4M
volatile bit pb_drive_square_pressed;   //push button flag for PB_DRVE_SQUARE
volatile bit pb_find_wall_pressed;      //push button flag for PB_FIND_WALL

void setup(void);                       //calls all other module setup functions
void main(void);                        //main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void refreshLcd(int distance);          //Refresh the LCD with ADC value and distance value

#endif	/* MAIN_H */

#include "main.h"

//set all push button flags to 0
volatile bit pb_scan_pressed = 0;
volatile bit pb_drive_4m_pressed = 0;
volatile bit pb_drive_square_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//calls all other module setup functions
void setup(void) {
    __delay_ms(5000);               //5 second startup delay to filter out irobot create serial crap

    setupInterrupt();               //calls pic interrupt setup function
    setupIRobot();                  //calls irobot create setup function
    setupSPI();                     //calls spi setup function for stepper motor control
    setupADC();                     //calls adc setup function for ir reading control
    setupLCD();                     //calls lcd setup function to allow writing to lcd
}

//main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void main(void) {
    setup();
    while (1) {
        //Refresh the LCD with ADC value and distance value
        refreshLcd(total_distance_travel);
        
        //checks for pb flags. If flag is set then perform a function
        if (pb_scan_pressed) {
            scanCw(400);
            moveCCW(scan_360_closest_step_count);
            adcDisplay();
            __delay_ms(5000);
            moveCCW(400 - scan_360_closest_step_count);
            pb_scan_pressed = 0;
        }
        if (pb_drive_4m_pressed) {
            moveStraight();
            pb_drive_4m_pressed = 0;
        }
        if (pb_drive_square_pressed) {
            moveSquare();
            pb_drive_square_pressed = 0;
        }
        if (pb_find_wall_pressed) {
            wallFollow();
            pb_find_wall_pressed = 0;
        }
    }
}

//Refresh the LCD with ADC value and distance value
void refreshLcd(int distance) {
    adcDisplay();
    distanceDisplay(distance);
}

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include "main.h"

#define DEBOUNCE_REQ_COUNT  10      //push button debounce required counter
#define TMR0_VAL            100     //start timer0 at 100

volatile unsigned int   hb_led_counter;     //timer0 interrupt overflow counter, used for heartbeat led
volatile unsigned int   lost_wall_timer;    //lost wall timer for wallFollow mode. Is only reset during wallFollow
volatile bit            pb_release;         //push button flag when no buttons are pressed

void setupInterrupt(void);          //sets up pic interrupt registers, also push button/led input/output for portB
void interrupt isr(void);           //interrupt function for the PIC. Uses timer0 interrupt overflow. Controls heartbeat LED. Controls push button debouncing and setting of push button flags
bit debounce(unsigned char count);  //takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT

#endif	/* INTERRUPT_H */
#include "interrupt.h"

volatile unsigned int   hb_led_counter = 0;     //timer0 interrupt overflow counter, used for heartbeat led
volatile unsigned int   lost_wall_timer = 0;    //lost wall timer for wallFollow mode. Is only reset during wallFollow
volatile bit            pb_release = 0;         //push button flag when no buttons are pressed

//sets up pic interrupt registers, also push button/led input/output for portB
void setupInterrupt(void) {
    TRISB = 0b00111110;
    PEIE = 1;
    GIE = 1;

    OPTION_REG = 0b00000100;
    TMR0IE = 1;
    ei();
}

//Interrupt function for the PIC. Uses timer0 interrupt overflow
//Controls heartbeat LED
//Controls push button debouncing and setting of push button flags
void interrupt isr(void) {
    unsigned char  pb_debounce_count = 0;  //push button debounce counter
    ser_int();

    if (T0IF) {
        T0IF = 0;
        TMR0 = TMR0_VAL;
        hb_led_counter++;
        lost_wall_timer++;

        //toggles heartbeat LED every 0.5 seconds
        if (hb_led_counter % 500 == 0) {
            HB_LED = !HB_LED;
            hb_led_counter = 0;
        }

        //checks for all 4 push buttons. If one is detected as pressed, start debounce counter. If debounce counter returns 1, then toggle push button flag.
        if (PB_SCAN || PB_DRIVE_4M || PB_DRIVE_SQUARE || PB_FIND_WALL){
            pb_debounce_count++;
            if (debounce(pb_debounce_count) && PB_SCAN) {
                pb_release = 0;
                pb_scan_pressed = 1;
            }
            if (debounce(pb_debounce_count) && PB_DRIVE_4M) {
                pb_release = 0;
                pb_drive_4m_pressed = 1;
            }
            if (debounce(pb_debounce_count) && PB_DRIVE_SQUARE) {
                pb_release = 0;
                pb_drive_square_pressed = 1;
            }
            if (debounce(pb_debounce_count) && PB_FIND_WALL) {
                pb_release = 0;
                pb_find_wall_pressed = 1;
            }
        }
        else {
            pb_debounce_count = 0;
            pb_release = 1;
        }
    }
}

//takes debounce counter and returns 1 if counter is > DEBOUNCE_REQ_COUNT
bit debounce(unsigned char count) {
    if (count > DEBOUNCE_REQ_COUNT && pb_release) {
        return 1;
    }
    else return 0;
}

#ifndef IROBOT_H
#define	IROBOT_H

#include "main.h"

//OPCODES
#define     START               128			//Create start command
#define     FULL                132			//Create full control command
#define     DRIVE               145			//Create Drive Direct command
#define     BUTTONS             18			//state of the create push buttons
#define     LEDS                139			//control the state of the create leds
#define     IRBYTE              17			//IR byte recieved by create
#define     SONG_PLAYING        37			//indicates if a song is currently being played
#define     PLAY_SONG           141			//play a song (0 - 15)
#define     SONG                140			//define a song
#define     SENSORS             142         //Create sensor command

//SENSOR PACKET_ID's for OPCODE 142
#define     DISTANCE            19          //distance in mm
#define     ANGLE               20          //angle in degrees
#define     VOLTAGE             22          //voltage in mV
#define     BATTERY_CHARGE      25          //battery current charge in mAh
#define     BATTERY_CAPACITY    26          //battery capacity in mAh
#define     BUMP_SENSOR          7

//IROBOT WHEEL SPEED DEFINES
#define     RIGHT_WHEEL_200     195         //right wheel speed in mm/s. right wheel slightly faster than left wheel, so compensate
#define     LEFT_WHEEL_200      200         //left wheel speed in mm/s

//IROBOT MANEUVERS
#define     SHARP_RIGHT()       drive(-80,LEFT_WHEEL_200);
#define     SHARP_LEFT()        drive(RIGHT_WHEEL_200,-80);
#define     SHARP_RIGHT2()      drive(-90,LEFT_WHEEL_200);
#define     SHARP_LEFT2()       drive(RIGHT_WHEEL_200,-90);
#define     SLOW_RIGHT()        drive(150,LEFT_WHEEL_200);
#define     SLOW_LEFT()         drive(RIGHT_WHEEL_200,150);
#define     SPIN_RIGHT()        drive(-RIGHT_WHEEL_200,LEFT_WHEEL_200);
#define     SPIN_LEFT()         drive(RIGHT_WHEEL_200,-LEFT_WHEEL_200);
#define     DRIVE_STRAIGHT()    drive(RIGHT_WHEEL_200,LEFT_WHEEL_200);
#define     DRIVE_BACKWARD()    drive(-RIGHT_WHEEL_200,-LEFT_WHEEL_200);
#define     DRIVE_STOP()        drive(0,0); __delay_ms(800);

int total_distance_travel;                      //Global variable which holds the total distance travelled, allowing to write to the LCD from any function.

void setupIRobot(void);                         //Starts robot and sets to Full mode. Initialises ser
void distanceDisplay(int distance);             //This function takes a distance value and writes it to the LCD, also writes mm driven after it. Distance is written whilst driving forward or square.
void moveStraight(void);                        //Drives the iRobot in a straight line for 4meters, refreshing the LCD as it goes.
void moveSquare(void);                          //Drives the iRobot in a straight line for 1m, turns left 90 degrees, then repeats 3 more times. This manuever creates a square.
void wallFollow(void);                          //Drives the iRobot around the maze. See large comment block in source file for more details.
void drive(int right_wheel, int left_wheel);    //driveDirect iRobot left and right wheels. function splits ints into 2 chars to send to iRobot
int distanceAngleSensor(char packet_id);        //returns 2 byte signed sensor data per packet id in datasheet
unsigned int sensorPacket(char packet_id);      //returns 2 byte unsigned sensor data per packet id in datasheet
unsigned char bumpPacket(char packet_id);       //returns 1 byte unsigned sensor data from the bump/wheel drop packet sensor
unsigned char cliffPacket(void);                //returns 1 byte unsigned sensor data for the cliff sensor. Cycles through all 4 cliff sensors
void writeBatteryStatusToLcd(void);             //Additional functionality to display battery status for 4 seconds on startup. Displays battery charge, capacity and voltage
int abs(int a);                                 //returns the absolute value of an int

#endif	/* IROBOT_H */

#include "iRobot.h"

int total_distance_travel = 0;

volatile bit wall_is_right_flag = 0;
volatile bit bump_cliff_flag = 0;

//Starts robot and sets to Full mode. Initialises ser
void setupIRobot(void) {
    ser_init();
    ser_putch(START);
    ser_putch(FULL);
}

//This function takes a distance value and writes it to the LCD, also writes mm driven after it. Distance is written whilst driving forward or square.
void distanceDisplay(int distance) {
    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(distance);
    lcdWriteString("mm driven    ");
}

//Drives the iRobot in a straight line for 4meters, refreshing the LCD as it goes.
void moveStraight(void){
    total_distance_travel = 0;
    
    DRIVE_STRAIGHT();                                               //Start driving forward at 200mm/s
    while (total_distance_travel < 4000) {                          //While loop continues until total distance travelled is 4000mm
        total_distance_travel += distanceAngleSensor(DISTANCE);     //Global variable stores total distance travelled from distancesSensor

        refreshLcd(total_distance_travel);
    }
    DRIVE_STOP();                                                   //Stops iRobot after 4m is achieved.
}

//Drives the iRobot in a straight line for 1m, turns left 90 degrees, then repeats 3 more times. This manuever creates a square.
void moveSquare(void) {
    int angle_turn = 0;
    int current_distance_travel = 0;
    int last_distance = 0;
    
    total_distance_travel = 0;

    char i = 4;
    for (i; i!=0; i--) {
        //Drive 1m forward
        DRIVE_STRAIGHT();                                                       //Start driving forward at 200mm/s
        while (current_distance_travel <= 995) {                                //While loop continues until total distance travelled is 1000mm
            current_distance_travel += distanceAngleSensor(DISTANCE);           //Global variable stores total distance travelled from distancesSensor
            total_distance_travel = current_distance_travel + last_distance;    //During first loop, last_distance shall be 0. Second loop it will be 1000m, Third loop 2000mm etc.

            refreshLcd(total_distance_travel);
        }

        //After 1m, stop then start turning on the spot
        DRIVE_STOP();
        SPIN_LEFT();

        //Turn 90 degrees, after 90 degrees exit the while loop.
        while(angle_turn < 90) {
            angle_turn += abs(distanceAngleSensor(ANGLE));
        }

        //After turning 90 degrees, stop
        DRIVE_STOP();

        //set current distance travelled to last distance travelled, clear variables for use in 'for' loop again
        last_distance += current_distance_travel;
        current_distance_travel = 0;
        angle_turn = 0;
    }
}

/*  
 *  Wall follow function. Starts with a 360 scan to find closest wall. Orientates robot to be facing the wall, then drives till it is 50cm from the wall.
 *  After 50cm from the wall, orientate robot to be perpendicular to the wall, depending on which direction it was initially. 
 *  If wall was initally to the right, once next to the wall, put wall the right again and vice versa.
 *  Once next to wall and perpendicular to it, initiate a forever while loop, using the IR sensor to change wheel speed to follow the wall around the whole maze.
 *  Bump sensors or cliff sensors can be triggered, causing the robot to pause for 10 seconds whilst it is manually maneuvered.
 */
void wallFollow (void){
    int angle = 0;
    int current_angle = 0;
    unsigned int distance = 0;
    
    total_distance_travel = 0;
    scanCcw(400);                           //CCWscan to find closest wall
    moveCW(scan_360_closest_step_count);    //Point IR sensor towards the wall.
    
    __delay_ms(500);
    
    /*  
     *  Depending on robot and IR sensor positions, orientate robot to face the wall in the quickest turn, either CW or CCW.
     *  Also set a flag which determines which direction to follow the wall for the rest of the procedure.
     */
    if (scan_360_closest_step_count > 200){
        angle = (400 - scan_360_closest_step_count) * 90 / 100;
        SPIN_LEFT();
        wall_is_right_flag = 0;                                                //from initial position, the wall is on the left
    } else {
        angle = scan_360_closest_step_count * 90 / 100;
        SPIN_RIGHT();
        wall_is_right_flag = 1;                                                //from initial position, the wall is on the right
    }
    
    //Turn 90 degrees
    while(angle > current_angle) {
        current_angle += abs(distanceAngleSensor(ANGLE));
    }

    //After turning 90 degrees, stop
    DRIVE_STOP();
    moveCW(400 - scan_360_closest_step_count);
    
    //drive forward till 60cm from wall
    DRIVE_STRAIGHT();
    while (distance > 50) {
        distance = getAdcDist(getAdc());
        adcDisplayQuick(distance);
    }
    DRIVE_STOP();
    
    //turn right or left 90 degrees
    if (wall_is_right_flag)  SPIN_LEFT();
    if (!wall_is_right_flag) SPIN_RIGHT();

    angle = 0;
    while(angle < 90) {
        angle += abs(distanceAngleSensor(ANGLE));
    }
    DRIVE_STOP();
    
    //After turning 90 degrees, move stepper motor 50 degrees right or lefts
    if (wall_is_right_flag)  moveCW(50);
    if (!wall_is_right_flag) moveCCW(50);
    
    bump_cliff_flag = 0;
    lost_wall_timer = 0;
    while (1) {
        unsigned char maneuver = 0;
        distance = getAdcDist(getAdc());    //continuously read the adc distance
        adcDisplayQuick(distance);          //write the distance using the quick lcd update function
        
        //Normal routine for robot wallFollow. Changes wheel speeds depending on distance to wall, or if wall has not been found for 5.5 seconds
        if (!bump_cliff_flag) {
            if (distance > 70 && lost_wall_timer >= 5500)   maneuver = 0;
            if (distance > 70 && lost_wall_timer < 5500)    maneuver = 1;
            if (distance < 48)                              maneuver = 2;
            if (distance >= 48 && distance <= 70)           maneuver = 3;
            
            if (wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_RIGHT(); break;                         //when wall is not found for certain time, turn sharp to the right
                    case 1: SLOW_RIGHT(); break;                           //when wall is at a nice distance, and it is not lost, slowly turn towards it
                    case 2: SHARP_LEFT2(); lost_wall_timer = 0; break;      //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT(); break;                       //when wall is at good distance, drive straight
                }
            }
            if (!wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_LEFT(); break;                          //when wall is not found for certain time, turn sharp to the left
                    case 1: SLOW_LEFT(); break;                            //when wall is at a nice distance, and it is not lost, slowly turn towards it.
                    case 2: SHARP_RIGHT2(); lost_wall_timer = 0; break;     //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT(); break;                       //when wall is at good distance, drive straight
                }
            }
            if (bumpPacket(BUMP_SENSOR) > 0 || cliffPacket() > 0) bump_cliff_flag = 1;    //when bump or cliff sensor is triggered, set bump flag
        }
        
        //If during normal routine, bump_flag is set, then stop, delay, and then continue normal routine
        if (bump_cliff_flag) {
            DRIVE_STOP();
            __delay_ms(10000);
            bump_cliff_flag = 0;
        }
    }
}
  
//driveDirect iRobot left and right wheels. function splits ints into 2 chars to send to iRobot
void drive(int right_wheel, int left_wheel) {
    char right_high = 0;
    char right_low = 0;
    char left_high = 0;
    char left_low = 0;

    right_high = (right_wheel >> 8);
    right_low = right_wheel;
    left_high = (left_wheel >> 8);
    left_low = left_wheel;

    ser_putch(DRIVE);
    ser_putch(right_high);
    ser_putch(right_low);
    ser_putch(left_high);
    ser_putch(left_low);
}

//returns 2 byte signed sensor data per packet id in datasheet
int distanceAngleSensor(char packet_id) {
    char high_byte, low_byte;
    int final_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	high_byte = ser_getch();
	low_byte = ser_getch();
    __delay_ms(15);
    
    return final_byte = (high_byte << 8 | low_byte);
}

//returns 2 byte unsigned sensor data per packet id in datasheet
unsigned int sensorPacket(char packet_id) {
    unsigned char high_byte, low_byte;
    unsigned int final_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	high_byte = ser_getch();
	low_byte = ser_getch();
     __delay_ms(15);

    return final_byte = (high_byte << 8 | low_byte);
}

//returns 1 byte unsigned sensor data from the bump/wheel drop packet sensor
unsigned char bumpPacket(char packet_id) {
    unsigned char bump_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	bump_byte = ser_getch();
     __delay_ms(15);

    return bump_byte;
}

//returns 1 byte unsigned sensor data for the cliff sensor. Cycles through all 4 cliff sensors
unsigned char cliffPacket(void){
    unsigned char cliff_byte;
    
    for (cliff_byte = 9; cliff_byte < 13; cliff_byte++){
        ser_putch(SENSORS);
        ser_putch(cliff_byte);
        
        if(ser_getch() > 0) return 1;
        __delay_ms(15);
    }
    return 0;
}

//Additional functionality to display battery status for 4 seconds on startup. Displays battery charge, capacity and voltage
void writeBatteryStatusToLcd(void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CHARGE));
    lcdWriteString("/");
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CAPACITY));
    lcdWriteString("mAh");

    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(sensorPacket(VOLTAGE));
    lcdWriteString("mV");

    __delay_ms(4000);
}

//returns the absolute value of an int
int abs(int a) {
    if(a < 0)
        return -a;
    return a;
}

#ifndef ADC_H
#define	ADC_H

#include "main.h"

void setupADC(void);                    //setup PIC registers for ADC to function
int adcDisplay(void);                   //takes average of 4 ADRESH/ADRESL readings and writes ADC distance in cm to LCD
void adcDisplayQuick(int distance);     //writes ADC distance to LCD but without multiple readings or averages
int getAdc(void);                       //converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdcDist(int adc_raw);            //takes converted adc variable and converts into cm according to characterisation equation

#endif	/* ADC_H */

#include "adc.h"

//setup PIC registers for ADC to function
void setupADC(void) {
    TRISA = 0b00111111;         //set all portA to input
    ADCON0 = 0b10000001;        //clock conversion FOSC/32, AD converter ON
    ADCON1 = 0b00000010;        //left justified, FOSC/32, pins 0-4 analogue

    __delay_us(50);
}

//takes average of 4 ADRESH/ADRESL readings and writes ADC distance in cm to LCD
int adcDisplay(void) {
    int average_adc = 0;

    char i = 4;
    for (i; i!=0; i--) {
        average_adc += getAdc();
    }
    average_adc = average_adc/4;

    lcdSetCursor(0x00);
    if (average_adc > 80) lcdWriteToDigitBCD(getAdcDist(average_adc));
    else lcdWriteString(">160");
    lcdWriteString("cm IR     ");
    
    return average_adc;
}

//displays a distance value to the LCD. This function takes in the distance which is already captured during other functions, and does not average the value. 
//Uses less time to write to LCD, which should not affect performance for manuevers that require precision.
void adcDisplayQuick(int distance) {    
    lcdSetCursor(0x00);    
    lcdWriteToDigitBCD(distance);
    lcdWriteString("cm IR     ");
}

//converts ADRESH and ADRESL into 1 int variable and returns this variable
int getAdc(void) {
    unsigned int adc_raw = 0;
    GO = 1;                                         //Starts ADC Conversion
    while(GO) continue;

    return adc_raw = (ADRESH << 2) | (ADRESL >> 6); //moves ADRESH and ADRESL to float
}

//takes converted adc variable and converts into cm according to characterisation equation
int getAdcDist(int adc_raw) {
    int adc_distance_cm;

    if (adc_raw > 500) {
        adc_distance_cm = 1/(((adc_raw)-376)/2520);
    }
    if (adc_raw <= 500) {
        adc_distance_cm = 1/(((adc_raw)-7)/11907);
    }

    return adc_distance_cm;
}

#ifndef STEPPER_H
#define	STEPPER_H

#include "main.h"

unsigned int scan_360_closest_step_count;   //counter to count how many half steps since closest scanned object
unsigned int old_adc_distance;              //variable to store closest reading of adc distance since push button press
unsigned char cw_control_byte;              //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char ccw_control_byte;             //stepper motor control byte for; enabled, counterclockwise, half-steps

void scanCw(unsigned int steps);    //Scan stepper motor clockwise. Finds the closest wall and stores this value to return to position later. Displays the distance per step to the LCD
void scanCcw(unsigned int steps);   //Scan stepper motor counter clockwise. Finds the closest wall and stores this value to return to position later. Displays the distance per step to the LCD
void findClosestWall(void);         //takes ADC and checks against old adc value, keeping the closest 'distance'
void moveCW(unsigned int steps);    //move stepper CW
void moveCCW(unsigned int steps);   //move stepper CCW
void resetADC(void);                //simple function to clear adc distance and step counters

#endif	/* STEPPER_H */

#include "stepper.h"

unsigned int scan_360_closest_step_count = 0;   //counter to count how many half steps since closest scanned object
unsigned int old_adc_distance = 0;                       //variable to store closest reading of adc distance since push button press

unsigned char cw_control_byte = 0b00001101;     //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char ccw_control_byte = 0b00001111;    //stepper motor control byte for; enabled, counterclockwise, half-steps
unsigned char off_control_byte = 0b00001100;    //stepper motor control byte for; off

/*
 * Scan stepper motor clockwise. Finds the closest wall and stores this value to return to position later.
 * Displays the distance per step to the LCD
 */
void scanCw(unsigned int steps) {
    resetADC();
    spi_transfer(cw_control_byte);

	for(steps; steps!=0; steps--){
        findClosestWall();
        SM_STEP();
        adcDisplay();
	}
    spi_transfer(off_control_byte);
    __delay_ms(2);
}
/*
 * Scan stepper motor counter clockwise. Finds the closest wall and stores this value to return to position later.
 * Displays the distance per step to the LCD
 */
void scanCcw(unsigned int steps) {
    resetADC();
    spi_transfer(ccw_control_byte);

	for(steps; steps!=0; steps--){
        findClosestWall();
        SM_STEP();
        adcDisplay();
	}
    spi_transfer(off_control_byte);
    __delay_ms(2);
}

//takes ADC and checks against old adc value, keeping the closest 'distance'
void findClosestWall(void) {
    int new_adc_distance = getAdc();

    if (new_adc_distance >= old_adc_distance) {
        old_adc_distance = new_adc_distance;
        scan_360_closest_step_count = 0;
    } else {
        scan_360_closest_step_count++;
    }
}

//move stepper CW
void moveCW(unsigned int steps) {
    spi_transfer(cw_control_byte);
    for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(1);
    }
    spi_transfer(off_control_byte);
}

//move stepper CCW
void moveCCW(unsigned int steps) {
    spi_transfer(ccw_control_byte);
    for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(1);
    }
    spi_transfer(off_control_byte);
}

//function to clear adc distance and step counters
void resetADC(void) {
    scan_360_closest_step_count = 0;
    old_adc_distance = getAdcDist(getAdc());
}

#ifndef LCD_H
#define	LCD_H

#include "main.h"

#define RS          RE0     //LCD Register Select
#define RW          RE1     //LCD Read Write
#define EN          RE2     //LCD Enable
#define LCD_DATA    PORTD   //LCD data pins are connected to Port D

void setupLCD(void);                            //function initalises the LCD module - check that ADCON1 setting doesn't conflict. Displays battery condition on startup
void lcdWriteControl(unsigned char databyte);   //write controls to LCD
void lcdWriteData(unsigned char databyte);      //write data to LCD
void lcdSetCursor(unsigned char address);       //move the LCD cursor to a particular location
void lcdWriteString(const char * s);            //write strings to LCD
void lcdWriteToDigitBCD(unsigned int data);     //function accepts char between 0 and 99999 and writes it to lcd display in seperate 5 digits

#endif	/* LCD_H */

#include "lcd.h"

//function initalises the LCD module - check that ADCON1 setting doesn't conflict
//function contains start-up battery checks written to LCD. Clears after 8 seconds
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

    writeBatteryStatusToLcd();      //Display battery condition on startup
    
    lcdWriteControl(0b00000001);    //clear display
    lcdSetCursor(0x40);
    lcdWriteString("0mm driven");
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
void lcdWriteString(const char * s) {
    while(*s) lcdWriteData(*s++);
}

//function accepts char between 0 and 99999 and writes it to lcd display in seperate 5 digits
void lcdWriteToDigitBCD(unsigned int data) {
    unsigned int    ones_digit;
    unsigned char   tens_digit, hundreds_digit, thousands_digit, ten_thousands_digit;

    //load number to be converted into OnesDigit
    ones_digit = data;
    tens_digit = 0;
    hundreds_digit = 0;
    thousands_digit = 0;
    ten_thousands_digit = 0;

    //Perform a BCD Conversion
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
    
    //write the digits to LCD
    if (data > 10000)   lcdWriteData((unsigned char) ten_thousands_digit + 48);
    if (data > 1000)    lcdWriteData((unsigned char) thousands_digit + 48);
    if (data > 100)     lcdWriteData((unsigned char) hundreds_digit + 48);
    if (data > 10)      lcdWriteData((unsigned char) tens_digit + 48);
    if (data > 0)       lcdWriteData((unsigned char) ones_digit + 48);
}

#ifndef SPI_H
#define	SPI_H

#include "main.h"

#define     SM_Control_Byte SSPBUF                  //SM_Control_Byte is SSPBUF
#define     SELECT_NONE()	RC0 = 0; RC1 = 0;
#define     SELECT_EEPROM()	RC0 = 0; RC1 = 1;
#define     SELECT_RF()		RC0 = 1; RC1 = 0;
#define     SELECT_SM()		RC0 = 1; RC1 = 1;
#define     SM_STEP()		RC2 = 1; NOP(); RC2 = 0;

void setupSPI(void);                                //setup SPI module and port c input/output pins. sets stepper motor to be controlled from pic
unsigned char spi_transfer(unsigned char data);     //Transfers data through SPI using SSPBUF/SM_Control_Byte. Note used interchangeably here for illustration purposes

#endif	/* SPI_H */

#include "spi.h"

//setup SPI module and port c input/output pins. sets stepper motor to be controlled from pic
void setupSPI(void){
    SSPSTAT = 0b01000000;
    SSPCON = 0b00100000;
    TRISC = 0b11010000;
    SELECT_SM();
}

//Transfers data through SPI using SSPBUF/SM_Control_Byte. Note used interchangeably here for illustration purposes
unsigned char spi_transfer(unsigned char data){
    unsigned char temp = 0;

    __delay_ms(10);
    SSPIF = 0;
    SSPBUF = data;

    while (SSPIF == 0);
    temp = SSPBUF;
    SSPIF = 0;
    __delay_ms(10);
    
    return temp;
}

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

#ifndef SER_H_
#define SER_H_

#include "main.h"

/* Valid buffer size value are only power of 2 (ex: 2,4,..,64,128) */
#define SER_BUFFER_SIZE     16
#define SER_FIFO_MASK       (SER_BUFFER_SIZE-1)

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
#include "ser.h"

unsigned char rxfifo[SER_BUFFER_SIZE];
volatile unsigned char rxiptr, rxoptr;
bank1 unsigned char txfifo[SER_BUFFER_SIZE];
volatile unsigned char txiptr, txoptr;
unsigned char ser_tmp;

bit ser_isrx(void) {
    if (OERR) {
        CREN = 0;
        CREN = 1;
        return 0;
    }
    return (rxiptr != rxoptr);
}

unsigned char ser_getch(void) {
    unsigned char c;

    while (ser_isrx() == 0)
        continue;

    GIE = 0;
    c = rxfifo[rxoptr];
    ++rxoptr;
    rxoptr &= SER_FIFO_MASK;
    GIE = 1;

    return c;
}

void ser_putch(unsigned char c) {
    while (((txiptr + 1) & SER_FIFO_MASK) == txoptr)
        continue;
    GIE = 0;
    txfifo[txiptr] = c;
    txiptr = (txiptr + 1) & SER_FIFO_MASK;
    TXIE = 1;
    GIE = 1;

    __delay_ms(5);
}

void ser_puts(const unsigned char * s) {
    while (*s)
        ser_putch(*s++);
}

void ser_puts2(unsigned char * s) {
    while (*s)
        ser_putch(*s++);
}

void ser_puthex(unsigned char v) {
    unsigned char c;

    c = v >> 4;
    if (c > 9) {
        ser_putch('A' - 10 + c);
    } else {
        ser_putch('0' + c);
    }

    c = v & 0x0F;
    if (c > 9) {
        ser_putch('A' - 10 + c);
    } else {
        ser_putch('0' + c);
    }
}

void ser_init(void) {
    TRISC |= 0b10000000; //set RC7 to input for RX
    TRISC &= 0b10111111; //set RC6 to output for TX
    BRGH = 1; //high speed

    SPBRG = 20; //57.6K @ 20MHz, SPBRG = (20MHz/(16*BAUD_RATE))-1;
    //	SPBRG=10;				//115.2K @ 20MHz, SPBRG = (20MHz/(16*BAUD_RATE))-1;

    TX9 = 0; //8 bits
    RX9 = 0; //

    SYNC = 0; //uart settings
    SPEN = 1;
    CREN = 1;
    TXIE = 0;
    RCIE = 1;
    TXEN = 1;
    PEIE = 1; //enable peripheral interrupts and global interrupts in main function


    rxiptr = rxoptr = txiptr = txoptr = 0;
}
