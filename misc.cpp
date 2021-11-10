// `misc.cpp`: Miscellaneous functions, including ultrasonic utils.
#include "misc.hpp"

#include <Arduino.h>
#include "pins.hpp"
#include "motor.hpp"


// Returns distance to the nearest object from the very front of the robot
// This is mostly from the sample code, except for the addition of the offset.
float get_distance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  return pulseIn(ULTRASONIC_ECHO, HIGH) / 58.0 - ULTRASONIC_OFFSET; // Account for the position of the ultrasonic relative to the front of the robot
}

// Make sure the robot is a precise distance from the ultrasonic target
void align_distance(float target, float precision) {
  // Keep track of how close it's been to the target over the last few cycles
  float err_memory = 0.0;
  while (true) {
    // Mostly-proportional controller.
    // The only difference from a standard proportional controller is that an additional 0.1 speed is added in the target direction.
    // This makes it so the motors don't stall too far from the target due to the speed being too low
    float dist = get_distance();
    float error = dist - target;
    float speeds = error*DISTANCE_KP + (error >= 0 ? 0.1 : -0.1);
    set_speeds(speeds, speeds);
    
    // Update the error memory
    err_memory = 0.9*err_memory + 0.1*error;
    
    // If it's been super close to the target recently, we're done
    if (-precision < err_memory && err_memory < precision) {
      return;
    }

    delay(5); // Delay so that the error memory isn't turned over too fast
  }
}

// Helper function to avoid having to use like 4 lines to set the LED
void set_led(int r, int g, int b) {
  leds[0].r = r;
  leds[0].g = g;
  leds[0].b = b;
  FastLED.show();
}
