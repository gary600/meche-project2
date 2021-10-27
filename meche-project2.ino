// This code is based on starter code provided by Elegoo and modifications made by Mark Bedillion

// Servo library
#include <Servo.h>

// Return value for get_line_state()
enum LineState {
  LINE_STATE_NONE, // no line detected
  LINE_STATE_LEFT, // the line is to the left
  LINE_STATE_CENTER, // the line is centered, or it's on a solid black surface
  LINE_STATE_RIGHT // the line is to the right
};

// Ultrasonic pins
#define ULTRASONIC_TRIG 13
#define ULTRASONIC_ECHO 12
#define SERVO_PIN 10 // Ultrasonic pan servo

// Motor pins
#define SPEED_L 6
#define SPEED_R 5
#define DIR_L 8
#define DIR_R 7
#define ENABLE 3 // Motor enable - must be HIGH for any motors to run

// Line follower pins
#define LINE_L A2
#define LINE_M A1
#define LINE_R A0

// Servo object for the ultrasonic servo
Servo ultrasonic_servo;

// Main motor speed function: input values on range -1.0 to 1.0 converted to correct output signals
void set_speeds(float speed_l, float speed_r) {
  _motor_speed(SPEED_L, DIR_L, speed_l);
  _motor_speed(SPEED_R, DIR_R, speed_r);
}
// Helper function to avoid code repetition
void _motor_speed(int speed_pin, int dir_pin, float speed) {
  if (speed >= 0) {
    analogWrite(speed_pin, speed*255.0);
    digitalWrite(dir_pin, HIGH); // HIGH is forward
  }
  else {
    analogWrite(speed_pin, -speed*255.0);
    digitalWrite(dir_pin, LOW);
  }
}
// Shorthand to stop motors
#define stop() set_speeds(0.0, 0.0)

// Returns distance measured by ultrasonic in cm
float get_distance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  return pulseIn(ULTRASONIC_ECHO, HIGH) / 58.0;
}

#define LINE_THRESHOLD 200 // Tuning value: line input greater than this is considered a line
// Returns the bot's position relative to the line
LineState get_line_state() {
  // true means line detected
  bool line_l = analogRead(LINE_L) > LINE_THRESHOLD;
  bool line_m = analogRead(LINE_M) > LINE_THRESHOLD;
  bool line_r = analogRead(LINE_R) > LINE_THRESHOLD;
  // Left/right states: don't care about middle sensor
  if (line_l && !line_r) {
    return LINE_STATE_LEFT;
  }
  if (!line_l && line_r) {
    return LINE_STATE_RIGHT;
  }
  // Centered state: either all 3 or just middle
  if ((line_l && line_m && line_r) || (!line_l && line_m && !line_r)) {
    return LINE_STATE_CENTER;
  }
  // Otherwise, line is lost
  return LINE_STATE_NONE;
}

// Prints a ton of diagnostics to Serial for debugging
void print_diagnostics() {
  Serial.print("LINE_L: ");
  Serial.println(analogRead(LINE_L));
  Serial.print("LINE_M: ");
  Serial.println(analogRead(LINE_M));
  Serial.print("LINE_R: ");
  Serial.println(analogRead(LINE_R));
  Serial.print("get_line_state(): ");
  switch (get_line_state()) {
    case LINE_STATE_NONE:
      Serial.println("LINE_STATE_NONE");
      break;
    case LINE_STATE_LEFT:
      Serial.println("LINE_STATE_LEFT");
      break;
    case LINE_STATE_CENTER:
      Serial.println("LINE_STATE_CENTER");
      break;
    case LINE_STATE_RIGHT:
      Serial.println("LINE_STATE_RIGHT");
      break;
  }
}

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
}

LineState last_turn = LINE_STATE_NONE;
void loop() {
  switch (get_line_state()) {
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
  print_diagnostics(); // Uncomment for serial spam
}
