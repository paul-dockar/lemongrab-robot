/* 
 * File:   iRobot.h
 * Author: JK
 *
 * Created on 21 April 2016, 2:21 PM
 */

#ifndef IROBOT_H
#define	IROBOT_H

#include "main.h"

#define		START                   128			//Create start command
#define		FULL			132			//Create full control command
#define		DRIVE			137			//Create Drive command
#define		BUTTONS			18			//state of the create push buttons
#define		LEDS			139			//control the state of the create leds
#define		IRBYTE			17			//IR byte recieved by create
#define		SONG_PLAYING            37			//indicates if a song is currently being played
#define		PLAY_SONG		141			//play a song (0 - 15)
#define		SONG			140			//define a song

void setupIRobot(void);
void drive(void);
void stop();
void test(void);

#endif	/* IROBOT_H */

