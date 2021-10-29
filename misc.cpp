#include "misc.hpp"

#include <Arduino.h>
#include "pins.hpp"
#include "line.hpp"


// Returns distance measured by ultrasonic in cm
float get_distance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  return pulseIn(ULTRASONIC_ECHO, HIGH) / 58.0;
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
