// This code is based on starter code provided by Elegoo and modifications made by Mark Bedillion

// External libraries
#include <Servo.h> // Servo driver
#include <FastLED.h> // for LED, mainly for debug. Go to Tools->Manage Libraries to install FastLED

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
  
  // ROUTINE: This is where we tell it the movements it has to do to complete the course!
  
  // Start and turn #1. Bias strongly to turn right so that it doesn't lose the turn at this speed
  follow_line_timed(1.0, 1500, true, 0.6);
  stop_motors();
  
  //TODO: Redo gates according to how they'll actually be on the course
  // Wait until gates are raised
  while (get_distance() < 50.0);
  
  // Fast on straight #1
  follow_line_timed(1.0, 1000, true, 0.0);
  stop_motors(); // Briefly stop to avoid wobbling on the next part (might not be necessary? needs more testing)
  delay(20);
  
  // Turn #2 and straight #2, take it slow (going too fast here seems to cause it to be super wobbly on the slalom)
  // Bias slightly right, because it sometimes tries to go left for some reason
  follow_line_timed(0.7, 1500, true, 0.2);
  
  // Slower on the slalom, and stop when the final wall is sensed
  // NOTE: update the distance (30.0) with the correct value once we have it!
  follow_line_until_near_wall(0.6, 30.0, true, 0.0);
  stop_motors();
  
  // Calibrate distance to wall precisely
  align_distance(30.0);
  stop_motors();
  
  // We're done!
}

void loop() {
  // Loop only runs once the routine is done, so Rainbow LED!
  leds[0] = CHSV(millis() / 10 % 255, 255, 255);
  FastLED.show();
}
