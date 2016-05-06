#include "stepper.h"

unsigned int scan_360_closest_step_count = 0;   //counter to count how many half steps since closest scanned object
unsigned int old_adc_distance = 0;                   //variable to store closest reading of adc distance since push button press

unsigned char cw_control_byte = 0b00001101;     //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char ccw_control_byte = 0b00001111;    //stepper motor control byte for; enabled, counterclockwise, half-steps
unsigned char off_control_byte = 0b00001100;

/*
 * Scan stepper motor clockwise. Finds the closest wall and stores this value to return to position later.
 * Displays the distance per step to the LCD
 */
void scanCw(unsigned int steps) {
    int adc_distance;
    resetADC();
    spi_transfer(cw_control_byte);

	for(steps; steps!=0; steps--){
        adc_distance = adcDisplayDistance();
        findClosestWall(adc_distance);
        SM_STEP();
	}
    spi_transfer(off_control_byte);
    __delay_ms(2);
}
/*
 * Scan stepper motor counter clockwise. Finds the closest wall and stores this value to return to position later.
 * Displays the distance per step to the LCD
 */
void scanCcw(unsigned int steps) {
    int adc_distance;
    resetADC();
    spi_transfer(ccw_control_byte);

	for(steps; steps!=0; steps--){
        adc_distance = adcDisplayDistance();
        findClosestWall(adc_distance);
        SM_STEP();
	}
    spi_transfer(off_control_byte);
    __delay_ms(2);
}

//takes ADC and checks against old adc value, keeping the closest 'distance'
void findClosestWall(int adc_distance) {
    unsigned int new_adc_distance = adc_distance;

    if (new_adc_distance <= old_adc_distance) {
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
