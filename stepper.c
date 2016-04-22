#include "stepper.h"

signed char cstep = 0;
signed int stepper_count = 0;
volatile bit cw_flag = 0;

//rotate stepper CW 360 degrees. scan adc each half step.
void scan360 (unsigned short steps){
    resetADC();
	for(steps; steps!=0; steps--){
        findClosestWall();
		switch (cstep){
            case 0:	PORTC = STEP1; cstep++; break;
			case 1:	PORTC = STEP2; cstep++; break;
            case 2:	PORTC = STEP3; cstep++; break;
            case 3:	PORTC = STEP4; cstep++; break;
            case 4:	PORTC = STEP5; cstep++; break;
            case 5:	PORTC = STEP6; cstep++; break;
            case 6:	PORTC = STEP7; cstep++; break;    
			case 7:	PORTC = STEP0; cstep = 0; break;
			default: PORTC = 0x00; break;
		}
		__delay_ms(10); 
	}
    moveCCW(scan_360_ccw_step_count);
}

//move stepper CW
void moveCW (unsigned short steps) {
	for(steps; steps!=0; steps--){
		switch (cstep){
            case 0:	PORTC = STEP1; cstep++; break;
			case 1:	PORTC = STEP2; cstep++; break;
            case 2:	PORTC = STEP3; cstep++; break;
            case 3:	PORTC = STEP4; cstep++; break;
            case 4:	PORTC = STEP5; cstep++; break;
            case 5:	PORTC = STEP6; cstep++; break;
            case 6:	PORTC = STEP7; cstep++; break;    
			case 7:	PORTC = STEP0; cstep = 0; break;
			default: PORTC = 0x00; break;
		}
		__delay_ms(10); 
	}
}

//move stepper CCW
void moveCCW (unsigned short steps) {
	for(steps; steps!=0; steps--){
		switch (cstep){
			case 0:	PORTC = STEP7; cstep = 7; break;
            case 1:	PORTC = STEP0; cstep--; break;
			case 2:	PORTC = STEP1; cstep--; break;
            case 3:	PORTC = STEP2; cstep--; break;
            case 4:	PORTC = STEP3; cstep--; break;
            case 5:	PORTC = STEP4; cstep--; break;
            case 6:	PORTC = STEP5; cstep--; break;
            case 7:	PORTC = STEP6; cstep--; break;
			default: PORTC = 0x00; break;
		}
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