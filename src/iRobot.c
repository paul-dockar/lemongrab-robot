#include "iRobot.h"

int total_distance_travel;
unsigned char victim_count;

volatile bit wall_is_right_flag;
volatile bit bump_cliff_flag;

volatile bit cliff_flag;
volatile bit bump_flag;
volatile bit virt_wall_flag;
volatile bit update_pos_flag;

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

void explore(void) {
    signed char direction_to_travel = 0;
    int angle_to_turn = 0;

    //set robot starting variables
    unsigned char *current_facing_direction = &local_map[1][1];
    unsigned char robot_x = 0;
    unsigned char robot_y = 1;
    unsigned char goal_x = 0;
    unsigned char goal_y = 3;
    unsigned char goal_number = 0;
    *current_facing_direction = 4;
    reset_flag = 1;
    exploring = 1;
    
    playSong(0);

    while (exploring) {
        if (reset_flag)  scanLocal(FULL_SCAN);
        if (!reset_flag) scanLocal(HALF_SCAN);

        //find direction to move next.
        //direction is either 1 (up), 2 (right), 3 (down), 4 (left), or -1 (dead-end)
        direction_to_travel = findPathAStar(robot_x, robot_y, goal_x, goal_y);

//                            lcdWriteControl(0b00000001);
//                            for (char x = 0; x < GLOBAL_X; x++) {
//                                    if (x == 0 || x == 2) lcdSetCursor(0x00);
//                                    if (x == 1 || x == 3) lcdSetCursor(0x40);
//                                for (char y = 0; y < GLOBAL_Y; y++) {
//                                    lcdWriteToDigitBCD(global_map[x][y]);
//                                    lcdWriteString(" ");
//                                }
//                                if (x == 1) __delay_ms(3000);
//                                if (x == 1) lcdWriteControl(0b00000001);
//                            }
//                            __delay_ms(3000);
//                            lcdWriteControl(0b00000001);
//                            lcdSetCursor(0x00);
//                            lcdWriteToDigitBCD(direction_to_travel);
//                            lcdWriteString(" direction");
//                            __delay_ms(2000);

        if (direction_to_travel == 0) {
            angle_to_turn = 0;
            if (goal_number == 0) {
                goal_x = 1;
                goal_y = 3;
                goal_number++;                  // keep exploring and set a new goal
            } else if (goal_number == 1) {
                goal_x = 3;
                goal_y = 0;
                goal_number++;                  // keep exploring and set a new goal
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
            angle_to_turn = 90 * (*current_facing_direction - direction_to_travel);
            if (angle_to_turn > 180) {
                angle_to_turn = angle_to_turn - 360;
            } else if (angle_to_turn < -180) {
                angle_to_turn = angle_to_turn + 360;
            }
            reset_flag = 0;
        }

        if (direction_to_travel != 0) {
             *current_facing_direction = direction_to_travel;                   //update facing direction
            driveAngle(angle_to_turn);                                          //spin to new direction
            driveStraight(950, robot_x, robot_y, *current_facing_direction);   //drive straight 1m
            switch (*current_facing_direction) {                                //update robot position
                case UP:    robot_x--; break;
                case RIGHT: robot_y++; break;
                case DOWN:  robot_x++; break;
                case LEFT:  robot_y--; break;
            }
        }
        
        if (victim_found_flag) victimCheck(robot_x, robot_y, &goal_x, &goal_y);
        
        if (update_pos_flag) {
            if (cliff_flag)     global_map[robot_x][robot_y] = CLIFF;
            if (bump_flag)      global_map[robot_x][robot_y] = BUMP;
            if (virt_wall_flag) global_map[robot_x][robot_y] = VIRTWALL;
            
            //revert robot position
            switch (*current_facing_direction) {
                case UP:    robot_x++; break;
                case RIGHT: robot_y--; break;
                case DOWN:  robot_x--; break;
                case LEFT:  robot_y++; break;
            }
            cliff_flag = bump_flag = virt_wall_flag = update_pos_flag = 0;
        }
    }
    playSong(0);
}

int driveStraight(int distance, char robot_x, char robot_y, char current_facing_direction) {
    int distance_traveled = 0;
    int distance_adc = 0;
    unsigned char maneuver = 0;

    move_stepper = 0;
    looking_straight = 0;
    looking_right = 0;
    looking_left = 1;
    moveCCW(100);
    ir_move_timer = 0;

    if (robot_x == 1 && robot_y == 2 && current_facing_direction == 2) slow_flag = 1;
    
    if (!slow_flag) {
        while (distance_traveled < distance) {
            distance_traveled += distanceAngleSensor(DISTANCE);
            distance_adc = adcDisplayDistance();

            if (bumpPacket(BUMP_SENSOR) > 0)            bump_flag = 1;
            if (cliffPacket() > 0)                      cliff_flag = 1;
            if (virtualWallPacket(VIRTWALL_SENSOR) > 0) virt_wall_flag = 1;
            if (victimSensor(IRBYTE) > 0)               victim_found_flag = 1;

            if (cliff_flag || bump_flag || virt_wall_flag) {
                update_pos_flag = 1;
                temp_global_info_flag = 1;

                DRIVE_STOP();
                distance_traveled = 0;

                DRIVE_BACKWARD();
                while (distance_traveled > -400) {
                    distance_traveled += distanceAngleSensor(DISTANCE);
                    adcDisplayQuick(distance_traveled);
                }

                distance_traveled = distance;
            }

            if (ir_move_timer > 200) {
                if (distance_adc >= 70)                         maneuver = 0;
                if (distance_adc < 48)                          maneuver = 1;
                if (distance_adc > 52 && distance_adc < 70)     maneuver = 2;
                if (distance_adc >= 48 && distance_adc <= 52)   maneuver = 3;

                if (looking_left) {
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
                        case 1: DRIVE_STOP(); distance_traveled = distance; break;
                        case 2: DRIVE_STRAIGHT_F(); break;
                        case 3: DRIVE_STRAIGHT_F(); break;
                    }
                }
            }
        }
    }
    if (slow_flag) {
        DRIVE_STRAIGHT_S();
        while (distance_traveled < distance) {
            distance_traveled += distanceAngleSensor(DISTANCE);

            if (cliffPacket() > 0) cliff_flag = 1; 

            if (cliff_flag) {
                update_pos_flag = 1;
                temp_global_info_flag = 1;

                DRIVE_STOP();
                distance_traveled = 0;

                DRIVE_BACKWARD();
                while (distance_traveled > -400) {
                    distance_traveled += distanceAngleSensor(DISTANCE);
                }

                distance_traveled = distance;
            }
        }
    }
    DRIVE_STOP();

    if (looking_left) moveCW(100);
    if (looking_right) moveCCW(100);
    slow_flag = 0;
    
    while (adcDisplayDistance() < 40) {
        DRIVE_BACKWARD();
    }
    DRIVE_STOP();
    
    return distance_traveled;
}

void victimCheck(unsigned char robot_x, unsigned char robot_y, unsigned char *goal_x, unsigned char *goal_y) {
    if (victim_count == 0) {
        victim_one_location = &global_map[robot_x][robot_y];
        playSong(1);
        victim_count++;
    } else if (victim_count == 1) {
        if (&global_map[robot_x][robot_y] != victim_one_location) {
            victim_two_location = &global_map[robot_x][robot_y];
            playSong(2);
            victim_count++;
            *goal_x = 0;
            *goal_y = 1;
        }
    }
    victim_found_flag = 0;
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

unsigned char victimSensor(unsigned char packet_id) {
    unsigned char return_byte = 0;
    
    ser_putch(SENSORS);
	ser_putch(packet_id);

	return_byte = ser_getch();
    __delay_ms(15);
    
    if (return_byte == 254) return 1;
    else return 0;
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
