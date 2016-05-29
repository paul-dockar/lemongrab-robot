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
    setupSPI();                     //calls spi setup function for stepper motor control
    setupADC();                     //calls adc setup function for ir reading control
    setupLCD();                     //calls lcd setup function to allow writing to lcd
    //setupExplore();
}

//main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void main(void) {
    setup();

    lcdWriteString(" ");
    lcdWriteToDigitBCD(EEPROM_ADDRESS_SONG_ONE);
    
    unsigned char temp = eepromRead(0x05);
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(temp);
    


    
    //Beat it
    for (char n = 0; n < 2; n++){
        playSong(0);
    }
    //Final Countdown
    for (char n = 0; n < 2; n++){
        playSong(1);
        playSong(2);
    }
    //Sandstorm
    for (char n = 0; n < 2; n++){
        playSong(3);
        playSong(4);
        playSong(5);
        playSong(3);
        playSong(6);
        playSong(3);
        playSong(6);
    }
    
    while (1) {
        //Refresh the LCD with ADC value and distance value
        refreshLcd(total_distance_travel);

        //checks for pb flags. If flag is set then perform a function
        if (pb_explore_pressed) {
            explore();
            //returnHome();
            pb_explore_pressed = 0;
        }
        if (pb_find_wall_pressed) {
            wallFollow();
            pb_find_wall_pressed = 0;
        }
    }
}

//Refresh the LCD with ADC value and distance value
void refreshLcd(int distance) {
    adcDisplayDistance();
    distanceDisplay(distance);
}