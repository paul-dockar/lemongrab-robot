#include "stepper.h"

unsigned char controlByte = 0;
unsigned short scan_360_ccw_step_count = 0;
unsigned short new_adc_distance = 0;
unsigned short closest_adc_distance = 0;

unsigned char CW_control_byte = SM_CLOCKWISE;
unsigned char CCW_control_byte = SM_COUNTER_CLOCKWISE;

//rotate stepper CW 360 degrees. scan adc each half step.
void scan360 (unsigned short steps){
    resetADC();
    spi_transfer(CW_control_byte);
    
	for(steps; steps!=0; steps--){
        findClosestWall();
        SM_STEP();
		__delay_ms(10);
	}
    moveCCW(scan_360_ccw_step_count);
}

//move stepper CW
void moveCW (unsigned short steps) {
    spi_transfer(CW_control_byte);
	for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(10);
	}
}

//move stepper CCW
void moveCCW (unsigned short steps) {
    spi_transfer(CCW_control_byte);
	for(steps; steps!=0; steps--){
        SM_STEP();
		__delay_ms(10);
	}
}

//takes ADC and checks against old adc value, keeping the closest 'distance'
void findClosestWall(void) {
    new_adc_distance = getAdc();

    if (new_adc_distance > closest_adc_distance) {
        closest_adc_distance = new_adc_distance;
        scan_360_ccw_step_count = 0;
    }
    scan_360_ccw_step_count++;
}

void resetADC(void) {
    scan_360_ccw_step_count = 0;
    closest_adc_distance = getAdc();
}