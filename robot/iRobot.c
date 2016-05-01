#include "iRobot.h"

int total_distance_travel = 0;

volatile bit wall_is_right_flag = 0;
volatile bit bump_flag = 0;
volatile bit cliff_flag = 0;
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
            angle_turn += abs(distanceAngleSensor(ANGLE));
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
    unsigned int distance = getAdcDist(getAdc());
    total_distance_travel = 0;
    scanCw(400);
    moveCCW(scan_360_closest_step_count);
    
    if (scan_360_closest_step_count > 200){
        angle = (400 - scan_360_closest_step_count) * 0.9;
        drive(-RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
        wall_is_right_flag = 1;                                                //from initial position, the wall is on the right
    } else {
        angle = scan_360_closest_step_count * 0.9;
        drive(RIGHT_WHEEL_VELOCITY,-LEFT_WHEEL_VELOCITY);
        wall_is_right_flag = 0;                                                //from initial position, the wall is on the left
    }

    //Turn 90 degrees
    while(angle >= current_angle) {
        current_angle += abs(distanceAngleSensor(ANGLE));
    }

    //After turning 90 degrees, stop
    drive(0,0);
    __delay_ms(500);
    
    moveCCW(400 - scan_360_closest_step_count);
    
    drive(RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
    
    //drive forward till 30cm from wall
    while (distance > 50) {
        distance = getAdcDist(getAdc());
        adcDisplayQuick(distance);
    }
    
    drive(0,0);
    __delay_ms(500);
    
    //turn right or left 90 degrees
    if (wall_is_right_flag)  drive(RIGHT_WHEEL_VELOCITY,-LEFT_WHEEL_VELOCITY);
    if (!wall_is_right_flag) drive(-RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
  
    
    int angle_turn = 0;
    while(angle_turn < 90) {
        angle_turn += abs(distanceAngleSensor(ANGLE));
    }
    
    drive(0,0);
    
    if (wall_is_right_flag) moveCW(50);
    if (!wall_is_right_flag) moveCCW(50);
    
    bump_flag = 0;
    cliff_flag = 0;
    while (1) {
        distance = getAdcDist(getAdc());
        adcDisplayQuick(distance);
        
        if (!bump_flag && !cliff_flag) {
            if (distance > 60 && lost_wall_timer < 5500) {
                if (wall_is_right_flag) {
                    drive(150,LEFT_WHEEL_VELOCITY);
                }
                if (!wall_is_right_flag) {
                    drive(RIGHT_WHEEL_VELOCITY,150);
                }
            } else if (distance < 48) {
                lost_wall_timer = 0;
                if (!wall_is_right_flag) {
                    drive(-90,LEFT_WHEEL_VELOCITY);
                }
                if (wall_is_right_flag) {
                    drive(RIGHT_WHEEL_VELOCITY,-90);
                }
            } else if (distance > 70 && lost_wall_timer >= 5500) {
                if (!wall_is_right_flag) {
                    drive(RIGHT_WHEEL_VELOCITY,-80);
                }
                if (wall_is_right_flag) {
                    drive(-80,LEFT_WHEEL_VELOCITY);
                }
            } else if (bumpPacket(BUMP_SENSOR) > 0 || cliffPacket() > 0) {
                bump_flag = 1;
                cliff_flag = 1;
            } else {
                drive(RIGHT_WHEEL_VELOCITY,LEFT_WHEEL_VELOCITY);
            }
        }
        if (bump_flag) {
            drive(0,0);
            __delay_ms(500);
            drive(-RIGHT_WHEEL_VELOCITY,-LEFT_WHEEL_VELOCITY);
            __delay_ms(2000);
            drive(0,0);
            __delay_ms(500);
            bump_flag = 0;
        }
        if (cliff_flag){
            lcdSetCursor (0x0B);
            lcdWriteString ("cliff");
        }
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

    return final_byte = (high_byte << 8 | low_byte);
}

unsigned char bumpPacket(char packet_id) {
    unsigned char bump_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	bump_byte = ser_getch();

    return bump_byte;
}

unsigned char cliffPacket(void){
    unsigned char cliff_byte;
    
    for (cliff_byte = 9; cliff_byte < 13; cliff_byte++){
        ser_putch(SENSORS);
        ser_putch(cliff_byte);
        
        if(ser_getch() > 0)
        {
            return 1;
        }
    }  
    return 0;
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