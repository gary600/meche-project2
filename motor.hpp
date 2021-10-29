#pragma once


void set_speeds(float l_speed, float r_speed);

// _motor_speed is not in here, since it's not intended to be used directly

// Shorthand macro to stop motors
#define stop() set_speeds(0.0, 0.0)
