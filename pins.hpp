/////////////////////////////////////////////////////////////////////////
// `pins.hpp`: Pin definitions and associated objects (servo and LED). //
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <Servo.h>
#include <FastLED.h>


// Ultrasonic pins
#define ULTRASONIC_TRIG 13
#define ULTRASONIC_ECHO 12
#define SERVO_PIN 10 // Ultrasonic pan servo
extern Servo ultrasonic_servo; // `extern` allows this global variable to be shared among files

// Motor pins: SPEED defines the speed based on PWM, and DIR defines the direction
#define SPEED_L 6
#define SPEED_R 5
#define DIR_L 8
#define DIR_R 7
#define ENABLE 3 // Motor enable - must be HIGH for any motors to run

// Line follower pins: higher value means less reflectance
#define LINE_L A2
#define LINE_M A1
#define LINE_R A0

// Misc
#define MODE_BUTTON 2
#define LED_PIN 4
extern CRGB leds[1]; // FastLED's LED object
