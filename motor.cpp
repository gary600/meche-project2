#include "motor.hpp"

#include <Arduino.h>
#include "pins.hpp"


// Helper function to avoid code repetition
void _motor_speed(int speed_pin, int dir_pin, float motor_speed) {
  if (motor_speed >= 0) {
    analogWrite(speed_pin, motor_speed*255.0);
    digitalWrite(dir_pin, HIGH); // HIGH is forward
  }
  else {
    analogWrite(speed_pin, -motor_speed*255.0);
    digitalWrite(dir_pin, LOW);
  }
}
// Main motor speed function: input values on range -1.0 to 1.0 converted to correct output signals
void set_speeds(float speed_l, float speed_r) {
  _motor_speed(SPEED_L, DIR_L, speed_l);
  _motor_speed(SPEED_R, DIR_R, speed_r);
}
