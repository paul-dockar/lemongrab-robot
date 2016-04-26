#ifndef IROBOT_H
#define	IROBOT_H

#include "main.h"

#define     START           128			//Create start command
#define     FULL            132			//Create full control command
#define     DRIVE           145			//Create Drive command
#define     BUTTONS         18			//state of the create push buttons
#define     LEDS            139			//control the state of the create leds
#define     IRBYTE          17			//IR byte recieved by create
#define     SONG_PLAYING    37			//indicates if a song is currently being played
#define     PLAY_SONG       141			//play a song (0 - 15)
#define     SONG            140			//define a song

void setupIRobot(void);
void drive(unsigned char opscode, unsigned char right_high, unsigned char right_low, unsigned char left_high, unsigned char left_low);
void stop(void);
void figureEightTest(void);

#endif	/* IROBOT_H */
