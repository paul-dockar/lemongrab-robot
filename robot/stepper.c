#include "stepper.h"

signed char cstep = 0;
signed int stepper_count = 0;
volatile bit cw_flag = 0;
unsigned char controlByte = 0;

unsigned short scan_360_ccw_step_count = 0;
float new_adc_distance = 0;
float closest_adc_distance = 0;

//rotate stepper CW 360 degrees. scan adc each half step.
void scan360 (unsigned short steps){
    resetADC();
    controlByte = 0b00001101;
    spi_transfer(controlByte);
	for(steps; steps!=0; steps--){
        findClosestWall();
        SM_STEP();
		__delay_ms(10);
	}
    moveCCW(scan_360_ccw_step_count);
}

//move stepper CW
void moveCW (unsigned short steps) {
	for(steps; steps!=0; steps--){
		__delay_ms(10);
	}
}

//move stepper CCW
void moveCCW (unsigned short steps) {
	for(steps; steps!=0; steps--){
		__delay_ms(10);
	}
}

//updates LCD step position
void lcdCW (void) {
    if ((stepper_count == 0) && (!cw_flag)){
        cw_flag = 1;
        lcdSetCursor(0X40);
        lcdWriteString("000 CW ");
    }
    if (cw_flag) {
        stepper_count++;
        writeStep(stepper_count);
    }
    else {
        stepper_count--;
        writeStep(stepper_count);
    }
}

//updates LCD step position
void lcdCCW (void) {
    if ((stepper_count == 0) && (cw_flag)){
        cw_flag = 0;
        lcdSetCursor(0X40);
        lcdWriteString("000 CCW");
    }
    if (cw_flag) {
        stepper_count--;
        writeStep(stepper_count);
    }
    else {
        stepper_count++;
        writeStep(stepper_count);
    }
}

//updates LCD step position
void writeStep (signed int step_count) {
    lcdSetCursor(0X40);
    lcdWriteToDigitBCD(step_count);
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