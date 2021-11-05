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

// Helper function: decides which way to turn to find a lost line. true == right
bool _decide_direction(float turn_memory, bool override_turn, float override_bias) {
  // Certainty: closer to 1 if the robot has been consistently turning in a direction recently, 0 if it hasn't
  // Approaches 1 as it turns tightly, approaches 0 as it goes straight
  // Numerically, it's the absolute value of turn_memory (explained in follow_line_forever)
  float certainty = turn_memory > 0.0 ? turn_memory : -turn_memory;
  
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
float _follow_line_step(float motor_speed, bool override_turn, float override_bias, float turn_memory) {
  //DEBUG: Display the turn memory certainty on the LED. more green=more certain, more red=less certain. (see _decide_direction for an explanation of certainty)
  float certainty = turn_memory > 0.0 ? turn_memory : -turn_memory;
  leds[0].r = (1.0-certainty)*255.0;
  leds[0].g = certainty*255.0;
  FastLED.show();

  // Do different things depending on where the line is relative to the robot
  switch (get_line_state()) {
    // If the line is centered, continue straight ahead, and advance turn memory
    case LINE_STATE_CENTER:
      set_speeds(motor_speed, motor_speed);
      turn_memory = turn_memory*(1.0-TURN_MEMORY_RATE);
      break;
    
    // If the line is to the left or right, turn that direction, and advance turn memory
    case LINE_STATE_LEFT:
      set_speeds(motor_speed*TURN_SECONDARY_SPEED_FACTOR, motor_speed*TURN_PRIMARY_SPEED_FACTOR);
      turn_memory = turn_memory*(1.0-TURN_MEMORY_RATE) - TURN_MEMORY_RATE;
      break;
    case LINE_STATE_RIGHT:
      set_speeds(motor_speed*TURN_PRIMARY_SPEED_FACTOR, motor_speed*TURN_SECONDARY_SPEED_FACTOR);
      turn_memory = turn_memory*(1.0-TURN_MEMORY_RATE) + TURN_MEMORY_RATE;
      break;
    
    // If the line is lost, decide which way to turn based on turn memory and the direction overidde provided. See _decide_direction()
    case LINE_STATE_NONE:
      // true == right
      if (_decide_direction(turn_memory, override_turn, override_bias)) {
        set_speeds(motor_speed*FIND_LINE_PRIMARY_SPEED_FACTOR, motor_speed*FIND_LINE_SECONDARY_SPEED_FACTOR);
      }
      else {
        set_speeds(motor_speed*FIND_LINE_SECONDARY_SPEED_FACTOR, motor_speed*FIND_LINE_PRIMARY_SPEED_FACTOR);
      }
      break;
  }
  // Return the updated turn memory
  return turn_memory;
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
  // Keep a "memory" of how it's been turning.
  // This is closer to 1.0 if it's been turning right consistently, and closer to -1.0 if it's been turning left consistently
  // The rate at which it updates is defined by TURN_MEMORY_RATE
  float turn_memory = 0.0;
  
  while (true) {
    // Do the line follow algorithm
    turn_memory = _follow_line_step(motor_speed, override_turn, override_bias, turn_memory);
    
    delay(TURN_PERIOD);
  }
}

// Follow the line until a certain amount of time has passed.
void follow_line_timed(float motor_speed, bool override_turn, float override_bias, int time_ms) {
  // Keep a "memory" of how it's been turning (see follow_line_forever for explanation)
  float turn_memory = 0.0;
  
  // Get the time at the start
  unsigned long start_time = millis();
  
  while (true) {
    // Do the line follow algorithm
    turn_memory = _follow_line_step(motor_speed, override_turn, override_bias, turn_memory);
    
    // End after enough time has passed
    if (millis() >= start_time + time_ms) {
      //DEBUG: stop between steps for timing debug
      //stop_motors();
      //delay(1000);
      return;
    }
    
    delay(TURN_PERIOD);
  }
}

// Follow line until the measured ultrasonic distance is below a certain value
void follow_line_until_near_wall(float motor_speed, bool override_turn, float override_bias, float distance) {
  // Keep a "memory" of how it's been turning (see follow_line_forever for explanation)
  float turn_memory = 0.0;
  
  while (true) {
    // Do the line follow algorithm
    turn_memory = _follow_line_step(motor_speed, override_turn, override_bias, turn_memory);
    
    // End if a wall is nearby
    if (get_distance() < distance) {
      return;
    }
    
    delay(TURN_PERIOD);
  }
}


#define LINE_KP 0.003
#define LINE_KI 0.0
#define LINE_KD 0.0

// A test implementation of PID to see if it's better
void follow_line_pid(float motor_speed) {
  int integral = 0; // for I term, arbitrary units
  int prev_err = 0; // for D term, arbitrary units
  unsigned long prev_time = millis(); // for I and D terms
  while (true) {
    int err = analogRead(LINE_R) - analogRead(LINE_L) ; // Let error be the difference in reading between left and right
    unsigned long now = millis();
    // Update integral
    integral += (now - prev_time) * err;
    // Find derivative
    float derivative = (float)(err - prev_err)/(float)(now - prev_time);

    // The resultant setpoint from PID. Negative means turn left. Range *should* be -1.0 to 1.0
    float setpoint = LINE_KP*(float)err + LINE_KI*(float)integral + LINE_KD*derivative;

    // Set motor speeds from setpoint
    if (err > 0.0 ) {
      set_speeds(motor_speed, motor_speed*(1.0-setpoint));
    }
    else {
      set_speeds(motor_speed*(1.0+setpoint), motor_speed);
    }

    Serial.print("err: "); Serial.println(err);
    Serial.print("derivative: "); Serial.println(derivative);
    Serial.print("integral: "); Serial.println(integral);
    Serial.print("set: "); Serial.println(setpoint);
  }
}
