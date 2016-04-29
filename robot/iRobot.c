#include "iRobot.h"

int total_distance_travel = 0;

volatile bit cw_flag = 0;

void setupIRobot(void) {
    ser_init();
    ser_putch(START);
    ser_putch(FULL);
}

void distanceDisplay(int distance) {
    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(distance);
    lcdWriteString("mm driven    ");
}

//move iRobot straight 4m
void moveStraight(void){
    total_distance_travel = 0;
    
    drive(RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
    while (total_distance_travel < 4000) {
        total_distance_travel += distanceAngleSensor(DISTANCE);

        refreshLcd(total_distance_travel);
    }
    drive(0,0);
}

//move iRobot in a 1m square shape
void moveSquare(void) {
    int angle_turn = 0;
    int current_distance_travel = 0;
    int last_distance = 0;
    
    total_distance_travel = 0;

    char i = 4;
    for (i; i!=0; i--) {
        drive(RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);

        //Drive 1m forward
        while (current_distance_travel <= 995) {
            current_distance_travel += distanceAngleSensor(DISTANCE);
            total_distance_travel = current_distance_travel + last_distance;

            refreshLcd(total_distance_travel);
        }

        //After 1m, stop then start turning on the spot
        drive(0,0);
        __delay_ms(800);
        drive(RIGHT_WHEEL_VELOCITY,-LEFT_WHEEL_VELOCITY);

        //Turn 90 degrees
        while(angle_turn < 90) {
            angle_turn += distanceAngleSensor(ANGLE);
            refreshLcd(total_distance_travel);
        }

        //After turning 90 degrees, stop
        drive(0,0);
        __delay_ms(800);

        //set distance travelled to last distance travelled, clear variables for use in 'for' loop again
        last_distance += current_distance_travel;
        current_distance_travel = 0;
        angle_turn = 0;
    }
}

void wallFollow (void){
    int angle = 0;
    int current_angle = 0;
    total_distance_travel = 0;
    scanCw(400);
    
    if(scan_360_closest_step_count > 200){
        angle = (400 - scan_360_closest_step_count) * 0.9;
        drive(-RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
        cw_flag = 1;
    } else {
        angle = scan_360_closest_step_count * 0.9;
        drive(RIGHT_WHEEL_VELOCITY,-LEFT_WHEEL_VELOCITY);
        cw_flag = 0;
    }

    //Turn 90 degrees
    while(angle >= current_angle) {
        current_angle += abs(distanceAngleSensor(ANGLE));
        refreshLcd(total_distance_travel);
    }

    //After turning 90 degrees, stop
    drive(0,0);
    __delay_ms(800);
    
    if (cw_flag) moveCCW(400 - scan_360_closest_step_count);
    if (!cw_flag) moveCW(400 - scan_360_closest_step_count);
    
    drive(RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
    
    scanCw(100);
    while (closest_adc_distance > 30) {
        scanCcw(200);
        scanCw(200);
    }
    
    drive(0,0);
    
    if (scan_counter < 0) {
        moveCW(abs(scan_counter));
    } else if (scan_counter >= 0) {
        moveCCW(abs(scan_counter));
    }
    
  }
  
//driveDirect iRobot left and right wheels. function splits ints into 2 chars to send to iRobot
void drive(int right_wheel, int left_wheel) {
    char right_high = 0;
    char right_low = 0;
    char left_high = 0;
    char left_low = 0;

    right_high = (right_wheel >> 8);
    right_low = right_wheel;
    left_high = (left_wheel >> 8);
    left_low = left_wheel;

    ser_putch(DRIVE);
    ser_putch(right_high);
    ser_putch(right_low);
    ser_putch(left_high);
    ser_putch(left_low);
}

//returns 2 byte signed sensor data per packet id in datasheet
int distanceAngleSensor(char packet_id) {
    char high_byte, low_byte;
    int final_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	high_byte = ser_getch();
	low_byte = ser_getch();

    __delay_ms(15);

    return final_byte = (high_byte << 8 | low_byte);
}

//returns 2 byte unsigned sensor data per packet id in datasheet
unsigned int sensorPacket(char packet_id) {
    unsigned char high_byte, low_byte;
    unsigned int final_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	high_byte = ser_getch();
	low_byte = ser_getch();

    __delay_ms(15);

    return final_byte = (high_byte << 8 | low_byte);
}

void writeBatteryStatusToLcd(void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CHARGE));
    lcdWriteString("/");
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CAPACITY));
    lcdWriteString("mAh");

    lcdSetCursor(0x40);
    lcdWriteToDigitBCDx6(sensorPacket(VOLTAGE));
    lcdWriteString("mV");

    __delay_ms(4000);               //display battery condition for 8 seconds
}

int abs(int a) {
    if(a < 0)
        return -a;
    return a;
}