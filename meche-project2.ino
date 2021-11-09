// `meche-project2.ino`: Main project file, including course routine.
// This code was originally based on starter code provided by Elegoo and modifications made by Mark Bedillion

// External libraries
#include <Servo.h> // Servo driver
#include <FastLED.h> // for debug output via the LED. Go to Tools->Manage Libraries to install FastLED

// Headers for project files
#include "pins.hpp"  // Pin definitions
#include "motor.hpp" // Motor functions (set_speeds mainly)
#include "line.hpp"  // Line follower functions
#include "misc.hpp"  // Miscellaneous functions (ultrasonic, etc)


// Objects for managing several devices
Servo ultrasonic_servo;
CRGB leds[1]; // The RGB LED on top. Managed with FastLED library


void setup() {
  // SETUP
  // Ultrasonic pins incl. servo
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  ultrasonic_servo.attach(SERVO_PIN, 500, 2400); // bind to servo object

  // Line follower pins
  pinMode(LINE_L, INPUT);
  pinMode(LINE_M, INPUT);
  pinMode(LINE_R, INPUT);
  
  // Motor pins
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);
  pinMode(SPEED_L, OUTPUT);
  pinMode(SPEED_R, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  
  // Mode button
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  
  // LED setup
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, 1);
  
  // Serial for debugging
  Serial.begin(9600);

  // Enable motors and stop them
  digitalWrite(ENABLE, HIGH);
  stop_motors();

  // Hold mode: To ensure precise timing of the start, movement only starts after releasing the MODE button
  while (digitalRead(MODE_BUTTON) == LOW);

  //follow_line_pid(0.3);

  // Do the actual course routine. This is in setup() so it only runs once.
  do_course();
}


// COURSE ROUTINE
// This is where we tell it the movements it has to do to complete the course!
// Most of he functions used here are in `line.cpp`, `motor.cpp`, and `misc.cpp`. See the files for explanations.
void do_course() {  
  // Start and turn #1. Bias strongly to turn right so that it doesn't lose the turn at this speed
  follow_line_timed(0.8, true, 0.9, 1600);
  stop_motors();
  
  leds[0].r = 255;
  leds[0].b = 0;
  FastLED.show();
  
  // Gates: go right up to the gate to trigger sensor
  align_distance(5.0, 0.2);
  // Wait till gates are raised
  stop_motors();
  while (get_distance() < 50.0);
  
  // Fast on straight #1, bias right in case we hit the turn early 
  follow_line_timed(1.0, true, 0.5, 1200);
  
  leds[0].r = 0;
  leds[0].b = 255;
  FastLED.show();
  
  // Turn #2, take it slower, bias right
  follow_line_timed(0.7, true, 0.7, 1000);
  
  leds[0].r = 255;
  leds[0].b = 0;
  FastLED.show();

  // Straight #2, fast
  follow_line_timed(0.9, true, 0.3, 400);
  
  leds[0].r = 0;
  leds[0].b = 255;
  FastLED.show();

  // Turn #3 and #4, bias right HARD
  follow_line_timed(0.7, true, 0.99, 2500);
  
  leds[0].r = 255;
  leds[0].b = 0;
  FastLED.show();
  
  // Slower on the slalom. Timed because we don't want to start sensing for the wall until very late because it'll sense the side of the gate assembly as the wall
  // No bias because the turns are too small to time separately(?)
  follow_line_timed(0.8, true, 0.0, 5300);
  
  leds[0].r = 0;
  leds[0].b = 255;
  FastLED.show();
  
  // Stop when the final wall is sensed. Bias right because it keeps turning left here
  follow_line_until_near_wall(0.8, true, 0.7, 15.0);
  stop_motors();
  
  leds[0].r = 255;
  leds[0].b = 0;
  FastLED.show();
  
  // Calibrate distance to wall precisely
  align_distance(10.0, 0.01);
  stop_motors();
  
  // We're done!
}


void loop() {
  // Loop only runs once the course routine is done, so Rainbow LED!
  leds[0] = CHSV(millis() / 10 % 255, 255, 255);
  FastLED.show();
}
