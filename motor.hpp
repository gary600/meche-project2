/////////////////////////////////////////////////////////
// `motor.hpp`: Associated header file for `motor.cpp` //
/////////////////////////////////////////////////////////

#pragma once


// See motor.cpp for explanation
void set_speeds(float l_speed, float r_speed);

// Shorthand macro to stop motors
#define stop_motors() set_speeds(0.0, 0.0)
