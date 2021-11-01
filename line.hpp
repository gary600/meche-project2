#pragma once

// Tuning value: line input value (0-1023) greater than this is considered a line
#define LINE_THRESHOLD 120 // Tested at 200 and 90, both had turnaround issues. 120 seems to be the most accurate

// Tuning values for line follow functions. These should all be >= 1.0 or we might overflow!
#define TURN_MEMORY_RATE 0.1 // The rate at which the turn memory updates. 1.0 means that it only knows about the last cycle, 0.0 means that it never updates with new cycle data.
#define FIND_LINE_SPEED_FACTOR 0.5
#define TURN_PRIMARY_SPEED_FACTOR 1.0
#define TURN_SECONDARY_SPEED_FACTOR 0.6
#define TURN_PERIOD 2 // Delay time at the end of each turn loop


// The 4 possible states the line can be in relative to the robot.
enum LineState {
  LINE_STATE_NONE, // no line detected
  LINE_STATE_LEFT, // the line is to the left
  LINE_STATE_CENTER, // the line is centered, or it's on a solid black surface
  LINE_STATE_RIGHT // the line is to the right
};

// See line.cpp for explanations of each function
LineState get_line_state();
void follow_line_timed(float motor_speed, int time_ms, bool override_turn, float override_bias);
void follow_line_until_near_wall(float motor_speed, float distance, bool override_turn, float override_bias);
void follow_line_forever(float motor_speed, bool override_turn, float override_bias);
