#include "iRobot.h"

void setupIRobot(void) {
    ser_init();
    ser_putch(START);
    ser_putch(FULL);
}

//move iRobot straight 4m
void moveStraight(void){
    int total_distance_travel = 0;

    drive(195,200);
    while (total_distance_travel < 4000) {
        total_distance_travel += distanceAngleSensor(DISTANCE);

        writeDistanceToLcd(total_distance_travel);
    }
    drive(0,0);
}

//move iRobot in a 1m square shape
void moveSquare(void) {
    int angle_turn = 0;
    int total_distance_travel = 0;
    int current_distance_travel = 0;
    int last_distance = 0;

    char i = 4;
    for (i; i!=0; i--) {
        drive(195,200);

        //Drive 1m forward
        while (current_distance_travel <= 995) {
            current_distance_travel += distanceAngleSensor(DISTANCE);
            total_distance_travel = current_distance_travel + last_distance;

            writeDistanceToLcd(total_distance_travel);
        }

        //After 1m, stop then start turning on the spot
        drive(0,0);
        __delay_ms(800);
        drive(195,-200);

        //Turn 90 degrees
        while(angle_turn < 90) {
            angle_turn += distanceAngleSensor(ANGLE);
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

void writeDistanceToLcd(int distance) {
    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(distance);
    lcdWriteString("mm driven    ");
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

void wallFollow (void){
    int angle = 0;
    int current_angle = 0;
    scan360(400);
   
        if(scan_360_closest_step_count > 200){
            angle = (0.9 * scan_360_closest_step_count) - 180;
            drive(-195,200);
        }
        else 
        {
            angle = -0.9 * scan_360_closest_step_count;
            drive(195,-200);
        }
        
            lcdSetCursor(0x00);
            lcdWriteToDigitBCD(angle);
            
            angle = abs(angle);
        
        //Turn 90 degrees
        while(angle > current_angle) {
            current_angle += distanceAngleSensor(ANGLE);
        }

        //After turning 90 degrees, stop
        drive(0,0);
        __delay_ms(800);

        //set distance travelled to last distance travelled, clear variables for use in 'for' loop again
        angle = 0;
}

int abs(int a)
{
	if(a < 0)
		return -a;
	return a;
}