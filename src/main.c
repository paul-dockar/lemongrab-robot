#include "main.h"

//set all push button flags to 0
volatile bit pb_explore_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//calls all other module setup functions
void setup(void) {
    __delay_ms(5000);               //5 second startup delay to filter out irobot create serial crap

    setupInterrupt();               //calls pic interrupt setup function
    writeSongToEeprom();            //if songs need to be updated, uncomment this and rebuild, upload using kirra with eeprom protection disabled
    setupIRobot();                  //calls irobot create setup function
    //setupSPI();                     //calls spi setup function for stepper motor control
    //setupADC();                     //calls adc setup function for ir reading control
    //setupLCD();                     //calls lcd setup function to allow writing to lcd
    //setupExplore();
}

//main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void main(void) {
    setup();

    //Beat it
    playSong(0);
    
    //Final Countdown
    playSong(1);
    playSong(2);
    
    //Sandstorm
    playSong(3);
    playSong(4);
    playSong(5);
    playSong(3);
    playSong(6);
    playSong(3);
    playSong(6);
    /*
    while (1) {
        //checks for pb flags. If flag is set then perform a function
        if (pb_explore_pressed) {
            explore();
            //returnHome();
            pb_explore_pressed = 0;
        }
    }*/
}