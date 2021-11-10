// `line.cpp`: Line follower functions. Most of the actual logic is done here.
#include "line.hpp"

#include <Arduino.h>
#include <FastLED.h>
#include "pins.hpp"
#include "motor.hpp"
#include "misc.hpp"


// HELPER FUNCTIONS

// Gets the bot's position relative to the line, according to the 4 possible states.
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

// A heuristic value for determining which way it should turn if it loses the line.
// This is interpolated toward 1.0 if it's been turning right consistently, and toward -1.0 if it's been turning left consistently
// Closer to 0.0 means that it hasn't been turning
float turn_memory = 0.0;

// Helper function: decides which way to turn to find a lost line. true == right
bool _decide_direction(bool override_turn, float override_bias) {
  // Certainty: closer to 1 if the robot has been consistently turning in a direction recently, 0 if it hasn't
  // Approaches 1 as it turns tightly, approaches 0 as it goes straight
  // Numerically, it's the absolute value of turn_memory
  float certainty = fabs(turn_memory);
  
  // If we're not certain about how we've been turning, use the direction we were given.
  if (certainty < override_bias) {
    return override_turn;
  }
  // Otherwise, we're certain enough that we've been turning this direction, so keep going that way
  else {
    return turn_memory > 0.0;
  }
}

// Helper function: does 1 step of the line follow algorithm, including setting speeds
float _follow_line_step(float motor_speed, bool override_turn, float override_bias) {
  //DEBUG: Display the turn memory certainty on the LED. more green=more certain, more red=less certain. (see _decide_direction for an explanation of certainty)
  //float certainty = fabs(turn_memory);
  //set_led((1.0-certainty)*255, certainty*255, 0);

  // Do different things depending on where the line is relative to the robot
  switch (get_line_state()) {
    // If the line is centered, continue straight ahead, and interpolate turn memory toward 0.0
    case LINE_STATE_CENTER:
      set_speeds(motor_speed, motor_speed);
      turn_memory = lerp(turn_memory, 0.0, TURN_MEMORY_RATE);
      break;
    
    // If the line is to the left or right, turn that direction, and interpolate turn memory towards 1.0 or -1.0 depending on direction
    case LINE_STATE_LEFT:
      set_speeds(motor_speed*TURN_SECONDARY_SPEED_FACTOR, motor_speed*TURN_PRIMARY_SPEED_FACTOR);
      turn_memory = lerp(turn_memory, -1.0, TURN_MEMORY_RATE);
      break;
    case LINE_STATE_RIGHT:
      set_speeds(motor_speed*TURN_PRIMARY_SPEED_FACTOR, motor_speed*TURN_SECONDARY_SPEED_FACTOR);
      turn_memory = lerp(turn_memory, 1.0, TURN_MEMORY_RATE);
      break;
    
    // If the line is lost, decide which way to turn based on turn memory and the direction overidde provided. See _decide_direction()
    case LINE_STATE_NONE:
      // true == right
      if (_decide_direction(override_turn, override_bias)) {
        set_speeds(motor_speed*FIND_LINE_PRIMARY_SPEED_FACTOR, motor_speed*FIND_LINE_SECONDARY_SPEED_FACTOR);
      }
      else {
        set_speeds(motor_speed*FIND_LINE_SECONDARY_SPEED_FACTOR, motor_speed*FIND_LINE_PRIMARY_SPEED_FACTOR);
      }
      break;
  }
}


// LINE FOLLOW FUNCTIONS

// These all follow the line until a certain condition is true (except follow_line_forever())
// If the line is lost, the algorithms above are used to try to recover.
// Most of the actual work in these is done by _follow_line_step.
// Explanations of common parameters:
//    motor_speed: the "target" motor speed to use in this movement
//    override_turn: the direction to turn if the line follower isn't certain on which way to turn
//    override_bias: the certainty value below which the line follower uses override_turn instead of the turn direction it thinks is correct
//      (See decide_direction for an explanation of certainty)

// Follow the line forever (mainly for debug purposes)
void follow_line_forever(float motor_speed, bool override_turn, float override_bias) {
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, override_turn, override_bias);
    
    delay(TURN_PERIOD);
  }
}

// Follow the line until a certain amount of time has passed.
void follow_line_timed(float motor_speed, bool override_turn, float override_bias, int time_ms) {
  // Get the time at the start
  unsigned long start_time = millis();
  
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, override_turn, override_bias);
    
    // End after enough time has passed
    if (millis() >= start_time + time_ms) {
      return;
    }
    
    delay(TURN_PERIOD);
  }
}

// Follow line until the measured ultrasonic distance is below a certain value
void follow_line_until_near_wall(float motor_speed, bool override_turn, float override_bias, float distance) {
  while (true) {
    // Do the line follow algorithm
    _follow_line_step(motor_speed, override_turn, override_bias);
    
    // End if a wall is nearby
    if (get_distance() < distance) {
      return;
    }
    
    delay(TURN_PERIOD);
  }
}
