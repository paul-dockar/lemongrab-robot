#include "iRobot.h"

int total_distance_travel = 0;

volatile bit wall_is_right_flag = 0;
volatile bit bump_cliff_flag = 0;

//Starts robot and sets to Full mode. Initialises ser
void setupIRobot(void) {
    ser_init();
    ser_putch(START);
    ser_putch(FULL);
    writeSongsToRobot();
}

//This function takes a distance value and writes it to the LCD, also writes mm driven after it. Distance is written whilst driving forward or square.
void distanceDisplay(int distance) {
    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(distance);
    lcdWriteString("mm driven    ");
}

/*
 *  Wall follow function. Starts with a 360 scan to find closest wall. Orientates robot to be facing the wall, then drives till it is 50cm from the wall.
 *  After 50cm from the wall, orientate robot to be perpendicular to the wall, depending on which direction it was initially.
 *  If wall was initally to the right, once next to the wall, put wall the right again and vice versa.
 *  Once next to wall and perpendicular to it, initiate a forever while loop, using the IR sensor to change wheel speed to follow the wall around the whole maze.
 *  Bump sensors or cliff sensors can be triggered, causing the robot to pause for 10 seconds whilst it is manually maneuvered.
 */
void wallFollow (void){
    int angle = 0;
    int current_angle = 0;
    unsigned int distance = 0;

    total_distance_travel = 0;
    scanCcw(400);                           //CCWscan to find closest wall
    moveCW(scan_360_step_count);            //Point IR sensor towards the wall.

    __delay_ms(500);

    /*
     *  Depending on robot and IR sensor positions, orientate robot to face the wall in the quickest turn, either CW or CCW.
     *  Also set a flag which determines which direction to follow the wall for the rest of the procedure.
     */
    if (scan_360_step_count > 200){
        angle = (400 - scan_360_step_count) * 90 / 100;
        SPIN_LEFT_F();
        wall_is_right_flag = 0;                                                //from initial position, the wall is on the left
    } else {
        angle = scan_360_step_count * 90 / 100;
        SPIN_RIGHT_F();
        wall_is_right_flag = 1;                                                //from initial position, the wall is on the right
    }

    //Turn 90 degrees
    while(angle > current_angle) {
        current_angle += abs(distanceAngleSensor(ANGLE));
    }

    //After turning 90 degrees, stop
    DRIVE_STOP();
    moveCW(400 - scan_360_step_count);

    //drive forward till 60cm from wall
    DRIVE_STRAIGHT_F();
    while (distance > 50) {
        distance = getAdcDist(getAdc());
        adcDisplayQuick(distance);
    }
    DRIVE_STOP();

    //turn right or left 90 degrees
    if (wall_is_right_flag)  SPIN_LEFT_F();
    if (!wall_is_right_flag) SPIN_RIGHT_F();

    angle = 0;
    while(angle < 90) {
        angle += abs(distanceAngleSensor(ANGLE));
    }
    DRIVE_STOP();

    //After turning 90 degrees, move stepper motor 50 degrees right or lefts
    if (wall_is_right_flag)  moveCW(50);
    if (!wall_is_right_flag) moveCCW(50);

    bump_cliff_flag = 0;
    lost_wall_timer = 0;
    while (1) {
        unsigned char maneuver = 0;
        distance = getAdcDist(getAdc());    //continuously read the adc distance
        adcDisplayQuick(distance);          //write the distance using the quick lcd update function

        //Normal routine for robot wallFollow. Changes wheel speeds depending on distance to wall, or if wall has not been found for 5.5 seconds
        if (!bump_cliff_flag) {
            if (distance > 70 && lost_wall_timer >= 5500)   maneuver = 0;
            if (distance > 70 && lost_wall_timer < 5500)    maneuver = 1;
            if (distance < 48)                              maneuver = 2;
            if (distance >= 48 && distance <= 70)           maneuver = 3;

            if (wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_RIGHT(); break;                         //when wall is not found for certain time, turn sharp to the right
                    case 1: SLOW_RIGHT(); break;                           //when wall is at a nice distance, and it is not lost, slowly turn towards it
                    case 2: SHARP_LEFT2(); lost_wall_timer = 0; break;      //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT_F(); break;                       //when wall is at good distance, drive straight
                }
            }
            if (!wall_is_right_flag) {
                switch (maneuver) {
                    case 0: SHARP_LEFT(); break;                          //when wall is not found for certain time, turn sharp to the left
                    case 1: SLOW_LEFT(); break;                            //when wall is at a nice distance, and it is not lost, slowly turn towards it.
                    case 2: SHARP_RIGHT2(); lost_wall_timer = 0; break;     //when wall is too close, turn sharp to the left. reset lost wall timer
                    case 3: DRIVE_STRAIGHT_F(); break;                       //when wall is at good distance, drive straight
                }
            }
            if (bumpPacket(BUMP_SENSOR) > 0 || cliffPacket() > 0 || virtualWallPacket(VIRTWALL_SENSOR) > 0) bump_cliff_flag = 1;    //when bump or cliff sensor is triggered, set bump flag
        }
        if (!wall_is_right_flag && bump_cliff_flag) SHARP_RIGHT();
        if (wall_is_right_flag && bump_cliff_flag) SHARP_LEFT();

    }
}

void explore(void) {
    signed char direction_to_travel = 0;
    int angle_to_turn = 0;

    //set robot starting variables
    unsigned char *current_facing_direction = &local_map[1][1];
    unsigned char robot_x = 0;
    unsigned char robot_y = 1;
    unsigned char goal_x = 0;
    unsigned char goal_y = 3;
    *current_facing_direction = 4;
    reset_flag = 1;
    exploring = 1;

    while (exploring) {
        if (reset_flag)  scanLocal(FULL_SCAN);
        if (!reset_flag) scanLocal(HALF_SCAN);

        //find direction to move next.
        //direction is either 1 (up), 2 (right), 3 (down), 4 (left), or -1 (dead-end)
        //direction_to_travel = findPathAStar(robot_x, robot_y, goal_x, goal_y);

                            lcdWriteControl(0b00000001);
                            for (char x = 0; x < GLOBAL_X; x++) {
                                    if (x == 0 || x == 2) lcdSetCursor(0x00);
                                    if (x == 1 || x == 3) lcdSetCursor(0x40);
                                for (char y = 0; y < GLOBAL_Y; y++) {
                                    lcdWriteToDigitBCD(global_map[x][y]);
                                    lcdWriteString(" ");
                                }
                                if (x == 1) __delay_ms(4000);
                                if (x == 1) lcdWriteControl(0b00000001);
                            }
                            __delay_ms(4000);
                            lcdWriteControl(0b00000001);

        if (direction_to_travel == 0) {
            if (victim_one && victim_two) {
                exploring = 0;
                return;
            } else {
                // keep exploring and set a new goal
                exploring = 1;
            }
        } else if (direction_to_travel == -1) {
            angle_to_turn = 180;
            switch (*current_facing_direction) {
                case 1: direction_to_travel = 3; break;
                case 2: direction_to_travel = 4; break;
                case 3: direction_to_travel = 1; break;
                case 4: direction_to_travel = 2; break;
            }
            global_map[robot_x][robot_y] = DEADEND;
        } else {
            //determine new direction
            angle_to_turn = 90 * abs((*current_facing_direction - direction_to_travel));
            if (angle_to_turn > 180) {
                angle_to_turn = angle_to_turn - 360;
            }
            reset_flag = 0;
        }

        driveAngle(angle_to_turn);          //spin desired direction

        //update robot position
        switch (direction_to_travel) {
            case 1: robot_x--; break;
            case 2: robot_y++; break;
            case 3: robot_x++; break;
            case 4: robot_y--; break;
        }

        *current_facing_direction = direction_to_travel;

        driveStraight(1000);                //drive straight 1m

    }

}
 

void returnHome(void) {


    //go to sleep when finished
    lcdWriteControl(0b00000000);
    asm("CLRWDT \n SLEEP");
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

int driveStraight(int distance) {
    int distance_traveled = 0;
    int distance_adc = 0;
    unsigned char maneuver = 0;

    move_stepper = 0;
    looking_straight = 0;
    looking_right = 0;
    looking_left = 1;
    moveCCW(100);
    ir_move_timer = 0;

    while (distance_traveled < distance) {
        DRIVE_STRAIGHT_F();
        distance_traveled += distanceAngleSensor(DISTANCE);
        distance_adc = adcDisplayDistance();
        
        if (bumpPacket(BUMP_SENSOR) > 0 || cliffPacket() > 0 || virtualWallPacket(VIRTWALL_SENSOR) > 0){
            DRIVE_STOP();
            distance_traveled = 0;
            DRIVE_BACKWARD();
            while (distance_traveled < 500) {
                distance_traveled += distanceAngleSensor(DISTANCE);
                adcDisplayQuick(distance_traveled);
            }
            distance_traveled =1000;
        }

        if (ir_move_timer > 200) {
            if (distance_adc >= 80)                         maneuver = 0;
            if (distance_adc < 48)                          maneuver = 1;
            if (distance_adc > 52 && distance_adc < 80)     maneuver = 2;
            if (distance_adc >= 48 && distance_adc <= 52)   maneuver = 3;

            if (looking_left) {
//                if (move_stepper) {
//                    moveCCW(200);
//                    move_stepper = 0;
//                    ir_move_timer = 0;
//                }
                switch (maneuver) {
                    case 0: looking_right = 1; looking_left = 0; move_stepper = 1; maneuver = 3; break;
                    case 1: SLOW_RIGHT(); break;
                    case 2: SLOW_LEFT(); break;
                    case 3: DRIVE_STRAIGHT_F(); break;
                }
            }
            if (looking_right) {
                if (move_stepper) {
                    moveCW(200);
                    move_stepper = 0;
                    ir_move_timer = 0;
                }
                switch (maneuver) {
                    case 0: looking_straight = 1; looking_right = 0; move_stepper = 1; break;
                    case 1: SLOW_LEFT(); break;
                    case 2: SLOW_RIGHT(); break;
                    case 3: DRIVE_STRAIGHT_F(); break;
                }
            }
            if (looking_straight) {
                if (move_stepper) {
                    moveCCW(100);
                    move_stepper = 0;
                    ir_move_timer = 0;
                }
                switch (maneuver) {
                    case 0: DRIVE_STRAIGHT_F(); break;
                    case 1: DRIVE_STOP(); distance_traveled = 1000; break;
                    case 2: DRIVE_STRAIGHT_F(); break;
                    case 3: DRIVE_STRAIGHT_F(); break;
                }
            }
        }
    }
    DRIVE_STOP();

    if (looking_left) moveCW(100);
    if (looking_right) moveCCW(100);

    return distance_traveled;
}

int driveAngle(int angle) {
    int angle_turned = 0;

    if (angle > 0) SPIN_LEFT_F();
    if (angle < 0) SPIN_RIGHT_F();

    while((angle_turned + 30) < abs(angle)) {
        angle_turned += abs(distanceAngleSensor(ANGLE));
    }

    if (angle > 0) SPIN_LEFT_S();
    if (angle < 0) SPIN_RIGHT_S();

    while(angle_turned < abs(angle)) {
    angle_turned += abs(distanceAngleSensor(ANGLE));
    }

    DRIVE_STOP();
    return angle_turned;
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

//returns 1 byte unsigned sensor data from the bump/wheel drop packet sensor
unsigned char bumpPacket(char packet_id) {
    unsigned char bump_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	bump_byte = ser_getch();
     __delay_ms(15);

    return bump_byte;
}

//returns 1 byte unsigned sensor data for the cliff sensor. Cycles through all 4 cliff sensors
unsigned char cliffPacket(void){
    unsigned char cliff_byte;

    for (cliff_byte = 9; cliff_byte < 13; cliff_byte++){
        ser_putch(SENSORS);
        ser_putch(cliff_byte);

        if(ser_getch() > 0) return 1;
        __delay_ms(15);
    }
    return 0;
}

unsigned char virtualWallPacket(char packet_id) {
    unsigned char virtual_wall_byte;

	ser_putch(SENSORS);
	ser_putch(packet_id);

	virtual_wall_byte = ser_getch();
     __delay_ms(15);

    return virtual_wall_byte;
}

//Additional functionality to display battery status for 4 seconds on startup. Displays battery charge, capacity and voltage
void writeBatteryStatusToLcd(void) {
    lcdSetCursor(0x00);
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CHARGE));
    lcdWriteString("/");
    lcdWriteToDigitBCD(sensorPacket(BATTERY_CAPACITY));
    lcdWriteString("mAh");

    lcdSetCursor(0x40);
    lcdWriteToDigitBCD(sensorPacket(VOLTAGE));
    lcdWriteString("mV");

    __delay_ms(4000);
}

void writeSongsToRobot (void) {
    unsigned char song_data = 0;
    unsigned char eeprom_address = EEPROM_ADDRESS_SONG_ONE;

    //write song one
    ser_putch(SONG);
    for (char i = 0; i < SONG_ONE_SIZE; i++) {
        song_data = eepromRead(eeprom_address);
        ser_putch(song_data);
        eeprom_address++;
    }
    eeprom_address = EEPROM_ADDRESS_SONG_TWO;
    //write song two
    ser_putch(SONG);
    for (char i = 0; i < SONG_TWO_SIZE; i++) {
        song_data = eepromRead(eeprom_address);
        ser_putch(song_data);
        eeprom_address++;
    }
    
    eeprom_address = EEPROM_ADDRESS_SONG_THREE;
    //write song three
    ser_putch(SONG);
    for (char i = 0; i < SONG_THREE_SIZE; i++) {
        song_data = eepromRead(eeprom_address);
        ser_putch(song_data);
        eeprom_address++;
    }
}

void playSong(unsigned char song_number) {
    while (bumpPacket(SONG_PLAYING) > 0){
        continue;
    }
    ser_putch(PLAY_SONG);
    ser_putch(song_number);
}

//returns the absolute value of an int
int abs(int a) {
    if(a < 0)
        return -a;
    return a;
}

signed char abs_char(signed char a) {
    if(a < 0)
        return -a;
    return a;
}
