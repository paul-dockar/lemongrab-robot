#ifndef IROBOT_H
#define	IROBOT_H

#include "main.h"

#define     START           128			//Create start command
#define     FULL            132			//Create full control command
#define     DRIVE           145			//Create Drive Direct command
#define     BUTTONS         18			//state of the create push buttons
#define     LEDS            139			//control the state of the create leds
#define     IRBYTE          17			//IR byte recieved by create
#define     SONG_PLAYING    37			//indicates if a song is currently being played
#define     PLAY_SONG       141			//play a song (0 - 15)
#define     SONG            140			//define a song

void setupIRobot(void);
void drive(char opscode, char right_high, char right_low, char left_high, char left_low);
void stop(void);
void figureEightTest(void);
void moveStraight(void);
#endif	/* IROBOT_H */
