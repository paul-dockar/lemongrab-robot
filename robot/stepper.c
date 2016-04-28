#include "stepper.h"

unsigned int scan_360_ccw_step_count = 0;     //counter to count how many half steps since closest scanned object
unsigned int new_adc_distance = 0;            //variable to store latest reading of adc distance
unsigned int closest_adc_distance = 0;        //variable to store closest reading of adc distance since push button press

unsigned char CW_control_byte = 0b00001111;     //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char CCW_control_byte = 0b00001101;    //stepper motor control byte for; enabled, counterclockwise, half-steps

//rotate stepper CW 360 degrees. scan adc distance each half step.
void scan360 (unsigned int steps){
    resetADC();
    spi_transfer(CW_control_byte);
    
	for(steps; steps!=0; steps--){
        //findClosestWall();
        SM_STEP();
		__delay_ms(10);
	}
    moveCCW(scan_360_ccw_step_count);
}

//move stepper CW
void moveCW (unsigned int steps) {
    spi_transfer(CW_control_byte);
	for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(10);
	}
}

//move stepper CCW
void moveCCW (unsigned int steps) {
    spi_transfer(CCW_control_byte);
	for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(10);
	}
}

//takes ADC and checks against old adc value, keeping the closest 'distance'
void findClosestWall(void) {
    new_adc_distance = getAdc();

        if (new_adc_distance < closest_adc_distance) {
        closest_adc_distance = new_adc_distance;
        scan_360_ccw_step_count = 0;
    }
    scan_360_ccw_step_count++;
}

//function to clear adc distance and step counters
void resetADC(void) {
    scan_360_ccw_step_count = 0;
    closest_adc_distance = getAdc();
}