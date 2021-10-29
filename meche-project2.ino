// This code is based on starter code provided by Elegoo and modifications made by Mark Bedillion

// Servo library
#include <Servo.h>

// Headers for project files
#include "pins.hpp"  // Pin definitions
#include "motor.hpp" // Motor functions (set_speeds mainly)
#include "line.hpp"  // Line follower functions
#include "misc.hpp"  // Miscellaneous functions (ultrasonic, etc)


// Servo object for the ultrasonic servo
Servo ultrasonic_servo;

void setup() {
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
  
  // Serial for debugging
  Serial.begin(9600);

  // Enable motors and stop them
  digitalWrite(ENABLE, HIGH);
  stop();


  follow_line_until_turned(0.8, 90.0, false, 0.0);
}

LineState last_turn = LINE_STATE_NONE;
void loop() {
  /*switch (get_line_state()) {
    case LINE_STATE_NONE:
      Serial.println("Line lost!");
      // turn in the last turn direction to try to find line again
      if (last_turn == LINE_STATE_LEFT) {
        set_speeds(-0.3, 0.3);
      }
      else {
        set_speeds(0.3, -0.3);
      }
      break;
    case LINE_STATE_LEFT: // Line is to the left, so steer left
      set_speeds(0.2, 0.5);
      last_turn = LINE_STATE_LEFT;
      break;
    case LINE_STATE_RIGHT:
      set_speeds(0.5, 0.2);
      last_turn = LINE_STATE_RIGHT;
      break;
    case LINE_STATE_CENTER:
      set_speeds(0.5, 0.5);
      break;
  }
  //delay(10);
  print_diagnostics(); // Uncomment for serial spam*/
}
