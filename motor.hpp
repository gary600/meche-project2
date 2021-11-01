#pragma once


// See motor.cpp for explanation
void set_speeds(float l_speed, float r_speed);

// Shorthand macro to stop motors
// Underscore is to avoid name conflict with some function FastLED exports
#define stop_motors() set_speeds(0.0, 0.0)
