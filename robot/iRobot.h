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
#define     VIRTWALL_SENSOR     13

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
unsigned char VirtualWallPacket(char packet_id);
void writeBatteryStatusToLcd(void);             //Additional functionality to display battery status for 4 seconds on startup. Displays battery charge, capacity and voltage
int abs(int a);                                 //returns the absolute value of an int

#endif	/* IROBOT_H */
