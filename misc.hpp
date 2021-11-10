// `misc.hpp`: Associated header file for `misc.cpp`
#pragma once


// Tuning values:

// The distance from the ultrasonic to the front of the robot
#define ULTRASONIC_OFFSET 1.5

// Kp for proportional controller for align_distance
#define DISTANCE_KP 0.01


// See misc.cpp for for explanations of each function
float get_distance();
void align_distance(float target, float precision);
void set_led(int r, int g, int b);
