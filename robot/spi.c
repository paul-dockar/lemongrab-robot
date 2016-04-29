#include "spi.h"

//setup SPI module and port c input/output pins. sets stepper motor to be controlled from pic
void setupSPI(void){
    SSPSTAT = 0b01000000;
    SSPCON = 0b00100000;
    TRISC = 0b11010000;
    SELECT_SM();
}

//Transfers data through SPI using SSPBUF/SM_Control_Byte. Note used interchangeably here for illustration purposes
unsigned char spi_transfer(unsigned char data){
    unsigned char temp = 0;

    __delay_ms(10);
    SSPIF = 0;
    SSPBUF = data;

    while (SSPIF == 0);
    temp = SSPBUF;
    SSPIF = 0;

    return temp;
}
