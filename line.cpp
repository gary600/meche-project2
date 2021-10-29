#include "line.hpp"

#include <Arduino.h>
#include "pins.hpp"
#include "motor.hpp"


// Returns the bot's position relative to the line
LineState get_line_state() {
  // true means line detected
  bool line_l = analogRead(LINE_L) > LINE_THRESHOLD;
  bool line_m = analogRead(LINE_M) > LINE_THRESHOLD;
  bool line_r = analogRead(LINE_R) > LINE_THRESHOLD;
  // Left/right states: don't care about middle sensor
  if (line_l && !line_r) {
    return LINE_STATE_LEFT;
  }
  if (!line_l && line_r) {
    return LINE_STATE_RIGHT;
  }
  // Centered state: either all 3 or just middle
  if ((line_l && line_m && line_r) || (!line_l && line_m && !line_r)) {
    return LINE_STATE_CENTER;
  }
  // Otherwise, line is lost
  return LINE_STATE_NONE;
}

#define TURN_MEMORY_RATE 0.5
#define FIND_LINE_SPEED 0.8
#define TURN_PRIMARY_SPEED 1.0
#define TURN_SECONDARY_SPEED 0.5

// Decides which way to turn to find a lost line. true == right
bool _decide_direction(float turn_memory, bool override_turn, float override_bias) {
  float certainty = turn_memory > 0.0 ? turn_memory : -turn_memory; // Closer to 1 if it has been turning in a direction consistently, 0 if it hasn't
  if (certainty < override_bias) {
    // Turn direction is uncertain from previous states. Use the given one instead.
    return override_turn;
  }
  else {
    // Turn direction is clear from previous states. Use that one.
    return turn_memory > 0.0;
  }
}
// Follow the line until turned a certain amount, given by the angle.
// If the line is lost, turn in the direction it's been turning already, unless 
void follow_line_until_turned(float motor_speed, float angle, bool override_turn, float override_bias) {
  // Keep a "memory" of how it's been turning
  float turn_memory = 0.0;
  while (true) {
    switch (get_line_state()) {
      // If the line is centered, continue straight ahead, advance turn memory
      case LINE_STATE_CENTER:
        set_speeds(motor_speed, motor_speed);
        turn_memory = turn_memory*TURN_MEMORY_RATE;
        break;
      
      // If the line is to the left or right, turn that direction, advance turn memory
      case LINE_STATE_LEFT:
        set_speeds(motor_speed*TURN_SECONDARY_SPEED, motor_speed*TURN_PRIMARY_SPEED);
        turn_memory = turn_memory*TURN_MEMORY_RATE - TURN_MEMORY_RATE;
        break;
      case LINE_STATE_RIGHT:
        set_speeds(motor_speed*TURN_PRIMARY_SPEED, motor_speed*TURN_SECONDARY_SPEED);
        turn_memory = turn_memory*TURN_MEMORY_RATE + TURN_MEMORY_RATE;
        break;
      
      // If the line is lost, decide which way to turn based on turn memory and the overrides
      case LINE_STATE_NONE:
        // true == right
        if (_decide_direction(turn_memory, override_turn, override_bias)) {
          set_speeds(motor_speed*FIND_LINE_SPEED, -motor_speed*FIND_LINE_SPEED);
        }
        else {
          set_speeds(-motor_speed*FIND_LINE_SPEED, motor_speed*FIND_LINE_SPEED);
        }
        break;
    }
    Serial.println(turn_memory);
    delay(10);
  }
}
