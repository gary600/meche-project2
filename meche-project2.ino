//This code is based on starter code provided by Elegoo
//www.elegoo.com

#include <Servo.h>  //servo library
Servo myservo;      // create servo object to control servo

//Define all of the pins used.  These are NOT up to us, but rather what Elegoo decided.  Don't change.
int Echo = 12;  
int Trig = 13; 

#define ENA 5
#define ENB 6
#define DIRA 8
#define DIRB 7
#define EN 3
//Line follower pins
#define LR A2
#define LM A1
#define LL A0

/******************************Helper functions*********************************************/
//Begin helper functions.  You should CALL these functions, but do not change them.  You DO NOT need to worry about the details inside the functions.

//The functions below set the left and right motor speeds and directions.
//Speed is an integer from 0 - 255 that determines the motor speed.
//Direction is 1 for forward and 0 for backwards.
void leftMotor(int lspeed, bool ldirection){
  analogWrite(ENA, lspeed);
  if (ldirection)
  {
    digitalWrite(DIRA, HIGH);
  }
  else
  {
    digitalWrite(DIRA, LOW);
  }
}
void rightMotor(int rspeed, bool rdirection){
  analogWrite(ENB, rspeed);
  if (rdirection)
  {
    digitalWrite(DIRB, HIGH);
  }
  else
  {
    digitalWrite(DIRB, LOW);
  }
}
//  The function below stops.  This is the same as just setting motor speeds to zero - not really necessary
void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
} 

//Ultrasonic distance measurement helper function - returns a float with distance in cm
float Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58.0;       
  return Fdistance;
}  

/*************************Setup*************************************************/
//You shouldn't need to touch this - it is merely setting up pins and stopping the motors at the start
void setup() { 
  myservo.attach(10,500,2400);  // attach servo on pin 10 to servo object
  Serial.begin(9600);     //For debugging
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(EN,OUTPUT);
  digitalWrite(EN,HIGH);
  stop();
} 
/********************************Loop - yours to edit!****************************************************************************/
//Below is some skeleton code that calls functions.  Your primary task is to edit this code to accomplish your goals.
void loop() {
   //Here is how you set the servo angle
    myservo.write(90);  //setservo position to angle; 90 is nominally straight in front
    delay(500); //Each time you change the servo, you should give it some time to reach the destination

    //Here is how you get the distance in cm
    float distance = Distance_test();  

    //Here is how you drive your car - this sample drives the car forward
    int rspeed = 255;
    int lspeed = 255;
    leftMotor(lspeed,1);  //Replace 1 with zero to reverse the direction for either motor
    rightMotor(rspeed,1);

    //Here is how you tell if the line following sensor is seeing the black tape
    //Check the values you read when on tape and off to see how to interpret these readings!
    int onTapeLeft = analogRead(LL);
    int onTapeRight = analogRead(LR);
    int onTapeMiddle = analogRead(LM);

    //Report variables back for your sanity
    Serial.print("Distance Reading: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("Left Line Sensor: ");
    Serial.println(onTapeLeft);
    Serial.print("Right Line Sensor: ");
    Serial.println(onTapeRight);
    Serial.print("Right Line Middle: ");
    Serial.println(onTapeMiddle);
}
