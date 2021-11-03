// This code is based on starter code provided by Elegoo and modifications made by Mark Bedillion

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

  // Line follower pins (Dr. B forgot this in his sample code...)
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
  
  // ROUTINE: This is where we tell it the movements it has to do to complete the course!
  
  // Start and turn #1. Bias strongly to turn right so that it doesn't lose the turn at this speed
  follow_line_timed(1.0, 1400, true, 0.6);
  stop_motors();
  
  // Go right up to the gate to trigger sensor
  align_distance(10.0);
  // Wait till gates are raised
  stop_motors();
  while (get_distance() < 50.0);
  // Wait a lil bit to avoid smashing camera on gate
  //TODO: remove this if they let us take the camera module off
  delay(1500);
  //TODO: Figure out if they decide to have gates lift up in sequence, and if so, account for that
  
  // Fast on straight #1
  follow_line_timed(1.0, 1500, true, 0.0);
  
  // Turn #2, take it slower
  // Bias right, because it sometimes tries to go left for some reason
  follow_line_timed(0.7, 1000, true, 0.7);

  // Straight #2, fast
  follow_line_timed(1.0, 500, true, 0.0);
  
  // Slower on the slalom. Timed because we don't want to start sensing for the wall until very late because it'll sense the side of the gate assembly as the wall
  // No bias because the turns are too small to time separately(?)
  follow_line_timed(0.7, 9500, true, 0.0);
  
  // Stop when the final wall is sensed. Bias right because it keeps turning left here
  follow_line_until_near_wall(0.8, 30.0, true, 0.7);
  stop_motors();
  
  // Calibrate distance to wall precisely
  // NOTE: update the distance (30.0) with the correct value once we have it!
  align_distance(30.0);
  stop_motors();
  
  // We're done!
}

void loop() {
  // Loop only runs once the routine is done, so Rainbow LED!
  leds[0] = CHSV(millis() / 10 % 255, 255, 255);
  FastLED.show();
}
