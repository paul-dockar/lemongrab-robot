#include "iRobot.h"

int total_distance_travel = 0;

volatile bit wall_is_right_flag = 0;
volatile bit bump_cliff_flag = 0;

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
    
    DRIVE_STRAIGHT();
    while (total_distance_travel < 4000) {
        total_distance_travel += distanceAngleSensor(DISTANCE);

        refreshLcd(total_distance_travel);
    }
    DRIVE_STOP();
}

//move iRobot in a 1m square shape
void moveSquare(void) {
    int angle_turn = 0;
    int current_distance_travel = 0;
    int last_distance = 0;
    
    total_distance_travel = 0;

    char i = 4;
    for (i; i!=0; i--) {
        DRIVE_STRAIGHT();
        //Drive 1m forward
        while (current_distance_travel <= 995) {
            current_distance_travel += distanceAngleSensor(DISTANCE);
            total_distance_travel = current_distance_travel + last_distance;
            refreshLcd(total_distance_travel);
        }

        //After 1m, stop then start turning on the spot
        DRIVE_STOP(); 
        
        SPIN_LEFT();
        //Turn 90 degrees
        while(angle_turn < 90) {
            angle_turn += distanceAngleSensor(ANGLE);
        }

        //After turning 90 degrees, stop
        DRIVE_STOP();

        //set distance travelled to last distance travelled, clear variables for use in 'for' loop again
        last_distance += current_distance_travel;
        current_distance_travel = 0;
        angle_turn = 0;
    }
}

void wallFollow (void){
    int angle = 0;
    int current_angle = 0;
    unsigned int distance = 0;
    
    total_distance_travel = 0;
    scanCcw(400);
    moveCW(scan_360_closest_step_count);
    
    __delay_ms(500);
    
    if (scan_360_closest_step_count >= 200){
        wall_is_right_flag = 0;                                                //from initial position, the wall is on the left
    } else {
        wall_is_right_flag = 1;                                                //from initial position, the wall is on the right
    }
    
    moveCW(400 - scan_360_closest_step_count);

    if (wall_is_right_flag)  moveCW(50);
    if (!wall_is_right_flag) moveCCW(50);
    
    bump_cliff_flag = 0;
    while (1) {
        unsigned char maneuver = 0;
        distance = getAdcDist(getAdc());                                            //continuously read the adc distance
        adcDisplayQuick(distance);                                                  //write the distance using the quick lcd update function
        
        if (!bump_cliff_flag) {                                                           //if bump_flag not set, do normal routine
            if (distance > 60 && lost_wall_timer >= 5500)   maneuver = 0;
            if (distance > 60 && lost_wall_timer < 5500)    maneuver = 1;
            if (distance < 48)                              maneuver = 2;
            if (distance >= 48 && distance <= 60)           maneuver = 3;
            
            if (wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_RIGHT(); break;                         //when wall is not found for certain time, turn sharp to the right
                    case 1: SLOW_RIGHT(); break;                           //when wall is at a nice distance, and it is not lost, slowly turn towards it
                    case 2: SHARP_LEFT();     lost_wall_timer = 0; break;  //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT(); lost_wall_timer = 0; break;  //when wall is at good distance, drive straight
                }
            }
            if (!wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_LEFT(); break;                           //when wall is not found for certain time, turn sharp to the left
                    case 1: SLOW_LEFT(); break;                            //when wall is at a nice distance, and it is not lost, slowly turn towards it.
                    case 2: SHARP_RIGHT();    lost_wall_timer = 0; break;  //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT(); lost_wall_timer = 0; break;  //when wall is at good distance, drive straight
                }
            }
            if (bumpPacket(BUMP_SENSOR) > 0 || cliffPacket() > 0) bump_cliff_flag = 1;    //when bump sensor is triggered, set bump flag
        }
        if (bump_cliff_flag) {                                                            //if bump_flag is set, stop, reverse, and then continue normal routine
            DRIVE_STOP();
            __delay_ms(10000);
            bump_cliff_flag = 0;
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
    lcdWriteToDigitBCD(sensorPacket(VOLTAGE));
    lcdWriteString("mV");

    __delay_ms(4000);               //display battery condition for 8 seconds
}

//returns the absolute value of an int
int abs(int a) {
    if(a < 0)
        return -a;
    return a;
}