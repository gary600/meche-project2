// This code is based on starter code provided by Elegoo and modifications made by Mark Bedillion

// External libraries
#include <Servo.h> // Servo driver

// Headers for project files
#include "pins.hpp"  // Pin definitions
#include "motor.hpp" // Motor functions (set_speeds mainly)
#include "line.hpp"  // Line follower functions
#include "misc.hpp"  // Miscellaneous functions (ultrasonic, etc)


// Servo object for the ultrasonic servo
Servo ultrasonic_servo;
// IMU object
//MPU6050 imu;

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
  
  // Mode button
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  
  // Serial for debugging
  Serial.begin(9600);

  // Enable motors and stop them
  digitalWrite(ENABLE, HIGH);
  stop();

  // Hold mode: To ensure precise timing of the start, movement only starts after releasing the MODE button
  while (digitalRead(MODE_BUTTON) == LOW);
  
  // MOVEMENT PROGRAMMING: This is where we tell it the movements it has to do!
  // Start and turn #1
  follow_line_timed(0.8, 1500, true, 0.0);
  stop();
  // Wait until gates are raised
  while (get_distance() < 50.0);
  // Fast on straight #1
  follow_line_timed(1.0, 1000, true, 0.0);
  // Turn #2 and straight #2, take it slow (going too fast here seems to cause it to be super wobbly on the slalom)
  follow_line_timed(0.8, 1500, true, 0.0);
  // A bit slower on the slalom, and stop when the final wall is sensed
  follow_line_until_near_wall(0.8, 30.0, true, 0.0);
  stop();
  //todo: ensure precise placement relative to wall
}

//LineState last_turn = LINE_STATE_NONE;
void loop() {
  // Loop will be empty in the final one since this just needs to run once
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
