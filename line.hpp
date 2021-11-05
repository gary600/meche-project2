// `line.hpp`: Associated header file for `line.cpp`
#pragma once


// Tuning values:

// A line input value (0-1023) greater than this is considered a line
#define LINE_THRESHOLD 200 // Seems most accurate. Tested at various values from 90 to 200

#define TURN_MEMORY_RATE 0.1 // The rate at which the turn memory updates. 1.0 means that it only knows about the last cycle, 0.0 means that it never updates with new cycle data.
#define TURN_PERIOD 2 // Delay time at the end of each turn loop

// Speed factors: the factors of the given speed to use for operations other than going straight
// Primary means the motor that goes faster, i.e. the left motor when turning right
#define TURN_PRIMARY_SPEED_FACTOR 1.0
#define TURN_SECONDARY_SPEED_FACTOR 0.6
#define FIND_LINE_PRIMARY_SPEED_FACTOR 0.5
#define FIND_LINE_SECONDARY_SPEED_FACTOR -0.5


// The 4 possible states the line can be in relative to the robot.
enum LineState {
  LINE_STATE_NONE, // no line detected
  LINE_STATE_LEFT, // the line is to the left of center
  LINE_STATE_CENTER, // the line is centered, or it's on a solid black surface
  LINE_STATE_RIGHT // the line is to the right of center
};


// See line.cpp for explanations of each function
LineState get_line_state();
void follow_line_forever(float motor_speed, bool override_turn, float override_bias);
void follow_line_timed(float motor_speed, bool override_turn, float override_bias, int time_ms);
void follow_line_until_near_wall(float motor_speed, bool override_turn, float override_bias, float distance);

void follow_line_pid(float motor_speed);
