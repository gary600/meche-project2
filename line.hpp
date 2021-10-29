#pragma once


// Tuning value: line input value (0-1023) greater than this is considered a line
#define LINE_THRESHOLD 100 // Tested at 200 and 90, both had turnaround issues. 100 seems to be the most accurate

// Return value for get_line_state()
enum LineState {
  LINE_STATE_NONE, // no line detected
  LINE_STATE_LEFT, // the line is to the left
  LINE_STATE_CENTER, // the line is centered, or it's on a solid black surface
  LINE_STATE_RIGHT // the line is to the right
};

LineState get_line_state();

void follow_line_until_turned(float motor_speed, float angle, bool override_turn, float override_bias);
