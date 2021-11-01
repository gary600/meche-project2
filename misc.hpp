#pragma once


// The distance from the ultrasonic to the front of the robot
#define ULTRASONIC_OFFSET 1.5

// Tuning value: Kp for proportional controller for align_distance
#define DISTANCE_KP 0.01


// See misc.cpp for for explanations of each function
float get_distance();
void print_diagnostics();
void align_distance(float target);
