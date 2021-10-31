#include "line.hpp"

#include <Arduino.h>
#include "pins.hpp"
#include "motor.hpp"
#include "misc.hpp"


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

// Helper funciton: does 1 step of the line follow algorithm
void _follow_line_step(float motor_speed, float* turn_memory, bool override_turn, float override_bias) {
  switch (get_line_state()) {
    // If the line is centered, continue straight ahead, advance turn memory
    case LINE_STATE_CENTER:
      set_speeds(motor_speed, motor_speed);
      *turn_memory = *turn_memory*(1.0-TURN_MEMORY_RATE);
      break;
    
    // If the line is to the left or right, turn that direction, advance turn memory
    case LINE_STATE_LEFT:
      set_speeds(motor_speed*TURN_SECONDARY_SPEED_FACTOR, motor_speed*TURN_PRIMARY_SPEED_FACTOR);
      *turn_memory = *turn_memory*(1.0-TURN_MEMORY_RATE) - TURN_MEMORY_RATE;
      break;
    case LINE_STATE_RIGHT:
      set_speeds(motor_speed*TURN_PRIMARY_SPEED_FACTOR, motor_speed*TURN_SECONDARY_SPEED_FACTOR);
      *turn_memory = *turn_memory*(1.0-TURN_MEMORY_RATE) + TURN_MEMORY_RATE;
      break;
    
    // If the line is lost, decide which way to turn based on turn memory and the overrides
    case LINE_STATE_NONE:
      // true == right
      if (_decide_direction(*turn_memory, override_turn, override_bias)) {
        set_speeds(motor_speed*FIND_LINE_SPEED_FACTOR, -motor_speed*FIND_LINE_SPEED_FACTOR);
      }
      else {
        set_speeds(-motor_speed*FIND_LINE_SPEED_FACTOR, motor_speed*FIND_LINE_SPEED_FACTOR);
      }
      break;
  }
}

// Follow the line until turned a certain amount, given by the angle.
// If the line is lost, turn in the direction it's been turning already, unless 
void follow_line_timed(float motor_speed, int time_ms, bool override_turn, float override_bias) {
  // Get the time at the start
  unsigned long start_time = millis();
  // Keep a "memory" of how it's been turning
  float turn_memory = 0.0;
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, &turn_memory, override_turn, override_bias);
    delay(10);
    // End loop after enough time has passed
    if (millis() >= start_time + time_ms) {
      return;
    }
  }
}

void follow_line_until_near_wall(float motor_speed, float distance, bool override_turn, float override_bias) {
  // Keep a "memory" of how it's been turning
  float turn_memory = 0.0;
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, &turn_memory, override_turn, override_bias);
    // End if a wall is nearby
    if (get_distance() < distance) {
      return;
    }
    delay(10);
  }
}

void follow_line_forever(float motor_speed, bool override_turn, float override_bias) {
  // Keep a "memory" of how it's been turning
  float turn_memory = 0.0;
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, &turn_memory, override_turn, override_bias);
    delay(10);
  }
}
