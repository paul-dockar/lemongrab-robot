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

//WHEEL SPEED DEFINES
#define     RIGHT_WHEEL_200     195         //right wheel speed in mm/s. right wheel slightly faster than left wheel, so compensate
#define     LEFT_WHEEL_200      200         //left wheel speed in mm/s

//WHEEL MANEUVERS
#define     SHARP_RIGHT()       drive(-80,LEFT_WHEEL_200);
#define     SHARP_LEFT()        drive(RIGHT_WHEEL_200,-80);
#define     SLOW_RIGHT()        drive(150,LEFT_WHEEL_200);
#define     SLOW_LEFT()         drive(RIGHT_WHEEL_200,150);
#define     SPIN_RIGHT()        drive(-100,100);
#define     SPIN_LEFT()         drive(100,-100);
#define     DRIVE_STRAIGHT()    drive(RIGHT_WHEEL_200,LEFT_WHEEL_200);
#define     DRIVE_BACKWARD()    drive(-RIGHT_WHEEL_200,-LEFT_WHEEL_200);
#define     DRIVE_STOP()        drive(0,0); __delay_ms(500);

int total_distance_travel;

void setupIRobot(void);
void distanceDisplay(int distance);
void moveStraight(void);
void moveSquare(void);
void wallFollow(void);
void drive(int right_wheel, int left_wheel);
int distanceAngleSensor(char packet_id);
unsigned int sensorPacket(char packet_id);
unsigned char bumpPacket(char packet_id);
unsigned char cliffPacket(void);
void writeBatteryStatusToLcd(void);
int abs(int a);

#endif	/* IROBOT_H */
