#include "main.h"

//set all push button flags to 0
volatile bit pb_explore_pressed = 0;
volatile bit pb_find_wall_pressed = 0;

//calls all other module setup functions
void setup(void) {
    __delay_ms(5000);               //5 second startup delay to filter out irobot create serial crap

    setupInterrupt();               //calls pic interrupt setup function
    setupIRobot();                  //calls irobot create setup function
    setupSPI();                     //calls spi setup function for stepper motor control
    setupADC();                     //calls adc setup function for ir reading control
    setupLCD();                     //calls lcd setup function to allow writing to lcd
    setupExplore();
}

//main program. starts by calling setup, then loops with pushbutton flag checks and displaying adc distance continuously
void main(void) {
    setup();
    while (1) {
        //Refresh the LCD with ADC value and distance value
        refreshLcd(total_distance_travel);

        //checks for pb flags. If flag is set then perform a function
        if (pb_explore_pressed) {
            explore();
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
