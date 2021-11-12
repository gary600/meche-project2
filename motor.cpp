///////////////////////////////////////////
// `motor.cpp`: Motor control functions. //
///////////////////////////////////////////

#include "motor.hpp"

#include <Arduino.h>
#include "pins.hpp"


// Helper function: Sets the speed of 1 side of the robot.
void _motor_speed(int speed_pin, int dir_pin, float motor_speed) {
  // Clamp to range -1 to 1 to prevent overflow
  motor_speed = motor_speed > 1.0 ? 1.0 : motor_speed;
  motor_speed = motor_speed < -1.0 ? -1.0 : motor_speed;
  // Positive means forwards
  if (motor_speed >= 0) {
    analogWrite(speed_pin, motor_speed*255.0);
    digitalWrite(dir_pin, HIGH); // HIGH is forward
  }
  // Negative means backwards
  else {
    analogWrite(speed_pin, -motor_speed*255.0);
    digitalWrite(dir_pin, LOW);
  }
}

// Main motor speed function: input values are of range -1.0 to 1.0
// These are converted to output signals of range 0 to 255 plus direction.
void set_speeds(float speed_l, float speed_r) {
  _motor_speed(SPEED_L, DIR_L, speed_l);
  _motor_speed(SPEED_R, DIR_R, speed_r);
}
