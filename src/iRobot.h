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
#define     RIGHT_WHEEL_FAST     300         //right wheel speed in mm/s. right wheel slightly faster than left wheel, so compensate
#define     LEFT_WHEEL_FAST      300         //left wheel speed in mm/s
#define     RIGHT_WHEEL_SLOW     100         //right wheel speed in mm/s. right wheel slightly faster than left wheel, so compensate
#define     LEFT_WHEEL_SLOW      100         //left wheel speed in mm/s

//IROBOT MANEUVERS
#define     SLOW_RIGHT()        drive(250,LEFT_WHEEL_FAST);
#define     SLOW_LEFT()         drive(RIGHT_WHEEL_FAST,250);
#define     SLOW_RIGHT2()       drive(200,LEFT_WHEEL_FAST);
#define     SLOW_LEFT2()        drive(RIGHT_WHEEL_FAST,200);

#define     SPIN_RIGHT_F()      drive(-RIGHT_WHEEL_FAST,LEFT_WHEEL_FAST);
#define     SPIN_LEFT_F()       drive(RIGHT_WHEEL_FAST,-LEFT_WHEEL_FAST);
#define     SPIN_RIGHT_S()      drive(-200,LEFT_WHEEL_SLOW);
#define     SPIN_LEFT_S()       drive(RIGHT_WHEEL_SLOW,-200);

#define     DRIVE_STRAIGHT_F()  drive(RIGHT_WHEEL_FAST,LEFT_WHEEL_FAST);
#define     DRIVE_STRAIGHT_S()  drive(140,140);
#define     DRIVE_BACKWARD()    drive(-RIGHT_WHEEL_FAST,-LEFT_WHEEL_FAST);
#define     DRIVE_STOP()        drive(0,0);

int total_distance_travel;                      //Global variable which holds the total distance travelled, allowing to write to the LCD from any function.
bit reset_flag;
bit exploring;
bit move_stepper;
bit slow_flag;
bit victim_found_flag;
unsigned char *victim_one_location;
unsigned char *victim_two_location;
unsigned char victim_count;
int total_distance_travel;

volatile bit cliff_flag;
volatile bit bump_flag;
volatile bit virt_wall_flag;
volatile bit update_pos_flag;

void setupIRobot(void);                         //Starts robot and sets to Full mode. Initialises ser
void distanceDisplay(int distance);             //This function takes a distance value and writes it to the LCD, also writes mm driven after it. Distance is written whilst driving forward or square.
void explore(void);
int driveStraight(int distance, char robot_x, char robot_y, char current_facing_direction);
void victimCheck(unsigned char robot_x, unsigned char robot_y);

void drive(int right_wheel, int left_wheel);    //driveDirect iRobot left and right wheels. function splits ints into 2 chars to send to iRobot
int driveAngle(int angle);
int distanceAngleSensor(char packet_id);        //returns 2 byte signed sensor data per packet id in datasheet
unsigned int sensorPacket(char packet_id);      //returns 2 byte unsigned sensor data per packet id in datasheet
unsigned char bumpPacket(char packet_id);       //returns 1 byte unsigned sensor data from the bump/wheel drop packet sensor
unsigned char cliffPacket(void);                //returns 1 byte unsigned sensor data for the cliff sensor. Cycles through all 4 cliff sensors
unsigned char virtualWallPacket(char packet_id);
unsigned char victimSensor(unsigned char packet_id);
void writeBatteryStatusToLcd(void);             //Additional functionality to display battery status for 4 seconds on startup. Displays battery charge, capacity and voltage
void writeSongsToRobot(void);
void playSong(unsigned char song_number);
int abs(int a);                                 //returns the absolute value of an int
signed char abs_char(signed char a);

#endif	/* IROBOT_H */
