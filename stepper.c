#include "stepper.h"

signed char cstep = 0;
signed int stepperCount = 0;
volatile bit cw_flag = 0;

//move stepper CW
void moveCW (char steps) {
	for(steps; steps!=0; steps--){
        lcdCW();
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
void moveCCW (char steps) {
	for(steps; steps!=0; steps--){
        lcdCCW();
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
    if ((stepperCount == 0) && (!cw_flag)){
        cw_flag = 1;
        lcdSetCursor(0X40);
        lcdWriteString("000 CW ");
    }
    if (cw_flag) {
        stepperCount++;
        writeStep(stepperCount);
    }
    else {
        stepperCount--;
        writeStep(stepperCount);
    }
}

//updates LCD step position
void lcdCCW (void) {
    if ((stepperCount == 0) && (cw_flag)){
        cw_flag = 0;
        lcdSetCursor(0X40);
        lcdWriteString("000 CCW");
    }
    if (cw_flag) {
        stepperCount--;
        writeStep(stepperCount);
    }
    else {
        stepperCount++;
        writeStep(stepperCount);
    }
}

//updates LCD step position
void writeStep (signed int stepCount) {
    lcdSetCursor(0X40);
    lcdWriteToDigitBCD(stepCount);
}