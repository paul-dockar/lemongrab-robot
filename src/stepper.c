#include "stepper.h"

unsigned char cw_control_byte = 0b00001101;     //stepper motor control byte for; enabled, clockwise, half-steps
unsigned char ccw_control_byte = 0b00001111;    //stepper motor control byte for; enabled, counterclockwise, half-steps
unsigned char off_control_byte = 0b00000000;

//move stepper CW
void moveCW(unsigned int steps) {
    spi_transfer(cw_control_byte);
    for(steps; steps > 0; steps--){
        SM_STEP();
		__delay_ms(1);
    }
    spi_transfer(off_control_byte);
}

//move stepper CCW
void moveCCW(unsigned int steps) {
    spi_transfer(ccw_control_byte);
    for(steps; steps > 0; steps--){
        SM_STEP();
		__delay_ms(1);
    }
    spi_transfer(off_control_byte);
}

//explore functions down here for now
void scanLocal(char scan_steps) {
    int adc_distance;
    char x = 1;
    char y = 0;
    int stepper_pos = 0;
    moveCCW(100);
    setupLocalMap();

    for (scan_steps; scan_steps > 0; scan_steps--) {
        __delay_ms(25);
        adc_distance = adcDisplayDistance();
        writeLocalMap(adc_distance, x, y);

        if (scan_steps > 1) moveCW(50);

        switch (stepper_pos) {
            case 0: x--; stepper_pos++; break;
            case 1: y++; stepper_pos++; break;
            case 2: y++; stepper_pos++; break;
            case 3: x++; stepper_pos++; break;
            case 4: x++; stepper_pos++; break;
            case 5: y--; stepper_pos++; break;
            case 6: y--; stepper_pos++; break;
            case 7: x--; stepper_pos++; break;
        }
    }
    stepper_pos = (stepper_pos * 50) - 150;
    moveCCW(stepper_pos);
}
