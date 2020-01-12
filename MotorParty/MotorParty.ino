// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>

#define TRIG_PIN A4
#define ECHO_PIN A0
#define MAX_DISTANCE_POSSIBLE 300 
#define MAX_SPEED 160
#define MAX_SPEED_OFFSET 20
#define MOTORS_CALIBRATION_OFFSET 10
#define COLL_DIST 30 
#define TURN_DIST COLL_DIST+10 
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);

int leftDistance, rightDistance;
int curDist = 0;
String motorSet = "";
int speedSet = 0;

int pos = 0;    
int presenceFire=0;
boolean fire = false;
int left=0;
int center=0;
int right=0;

// DC motor on M1
AF_DCMotor motor1(1,MOTOR12_8KHZ);
// DC motor on M3
AF_DCMotor motor2(3,MOTOR12_8KHZ);
// DC hobby servo
Servo servo1;
Servo servo2;
int analogPin0=A1;
int analogPin3=A3;
int analogPin5=A5;
int analogValLeft;
int analogValCenter;
int analogValRight;
int pump=A2;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Fire Robot ON!");

 

  // turn on motor #2
  motor1.setSpeed(200);
  motor1.run(RELEASE);

  motor2.setSpeed(200);
  motor2.run(RELEASE);

 // turn on servo
  servo1.attach(9);
  servo1.write(90);
  servo2.attach(10);
  servo2.write(90);  
  pinMode(pump,OUTPUT);

  delay(1000);

  
  
}

void put_off_fire()
{
    delay (500);
 
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    
   digitalWrite(pump, HIGH); 
   delay(500);

   for(pos =90; pos>=50; pos-=1){
    servo1.write(pos);
    delay(10);
   }
    
    for (pos = 50; pos <= 130; pos += 1) { 
    servo1.write(pos); 
    delay(10);  
  }
  for (pos = 130; pos >= 50; pos -= 1) { 
    servo1.write(pos); 
    delay(10);
  }
  for(pos=50; pos<=90 ; pos+=1){
    servo1.write(pos);
    delay(10);
  }
  
  digitalWrite(pump,LOW);
  servo1.write(90);
  
  fire=false;
}

void loop()
{


  
  
  analogValLeft=analogRead(analogPin0);
  Serial.print("Left: ");
  Serial.println(analogValLeft);
  analogValCenter=analogRead(analogPin3);
  Serial.print("Center: ");
  Serial.println(analogValCenter);
  analogValRight=analogRead(analogPin5);
  Serial.print("Right: ");
  Serial.println(analogValRight);
  //motor1.run(FORWARD);
  //motor2.run(BACKWARD);
  

  
  motor1.run(RELEASE);
  motor2.run(RELEASE);

  if(analogValLeft>900 && analogValCenter>900 && analogValRight>900) //no fire
  {
      motor1.run(RELEASE);
      motor2.run(RELEASE);
  }

  
  

   if(right && analogValRight<900)
        Serial.println("Right is min");
   if(left)
        Serial.println("Left is min");
   if(center)
         Serial.println("Center is min");

   if( analogValLeft<850 || analogValRight<850 ||  analogValCenter<850)
        presenceFire=1;
  

   if(presenceFire==0)
          checkForward();
   else
      Serial.println("fire is present, moving controled by fire sensors");

 while(presenceFire==1)
{
    analogValLeft=analogRead(analogPin0);
  Serial.print("Left: ");
  Serial.println(analogValLeft);
  analogValCenter=analogRead(analogPin3);
  Serial.print("Center: ");
  Serial.println(analogValCenter);
  analogValRight=analogRead(analogPin5);
  Serial.print("Right: ");
  Serial.println(analogValRight);

    if(analogValLeft < analogValRight && analogValLeft < analogValCenter)
  {
      left=1;
      center=0;
      right=0;
  }


  if(analogValCenter < analogValRight && analogValCenter < analogValLeft)
  {
      center=1;
      left=0;
      right=0;
  }
  
   if(analogValRight < analogValCenter && analogValRight < analogValLeft)
   {
      right=1;
      left=0;
      center=0;
   }

   
   if(left &&  analogValLeft<750)
   {
      motor1.setSpeed(150);
      motor2.setSpeed(160);
      motor1.run(RELEASE);
      motor2.run(BACKWARD);
      delay(300);
   }
    if(right && analogValRight<750)
   {
      motor1.setSpeed(160);
      motor2.setSpeed(150);
      motor1.run(FORWARD);
      motor2.run(RELEASE);
      delay(300);
   }

  if(center && analogValCenter<750)
  {
      motor1.setSpeed(160);
      motor2.setSpeed(160);

      while(analogValCenter >50 && analogValCenter<750 && center)
      {
        motor1.run(FORWARD);
        motor2.run(BACKWARD);
        delay(100);
     
        analogValCenter=analogRead(analogPin3);
      }
      if(analogValCenter<50)
          fire=true;
          

  }

  if(fire==true){
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    Serial.println("FIREEEE");
  }
  if(fire==true){
    put_off_fire();
    presenceFire=0;
  }
  motor1.run(RELEASE);
  motor2.run(RELEASE);
   
//  put_off_fire();
 // delay(2000);
}
  
}

void checkForward(){
  servo2.write(90);
  delay(90);
  curDist=readPing();
  if(curDist < COLL_DIST)
    changePath();
  moveForward();
  delay(500);
}

void changePath(){
  moveStop();
  servo2.write(10);
    delay(500);
    rightDistance = readPing();
    delay(500);
    servo2.write(180);
    delay(700);
    leftDistance = readPing();
    delay(500);
    servo2.write(90);
    delay(100);
    compareDistance();
}

void compareDistance(){
  if(leftDistance > rightDistance && leftDistance> COLL_DIST){
    turnLeft();
  }
  else if(rightDistance > leftDistance && rightDistance> COLL_DIST){
    turnRight();
  }
  else{
    turnAround();
  }
}

int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if(cm==0)
    cm=500;
  return cm;
}

void moveStop()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void moveForward(){
  motorSet="FORWARD";
     motor1.setSpeed(150);
   motor2.setSpeed(150+MOTORS_CALIBRATION_OFFSET);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  for(speedSet=0; speedSet<MAX_SPEED; speedSet += 2)
  {
   motor1.setSpeed(150);
   motor2.setSpeed(150+MOTORS_CALIBRATION_OFFSET);
   delay(5); 
  }
}

void turnRight()
{
  motorSet="RIGHT";
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor2.setSpeed(speedSet+MAX_SPEED_OFFSET);
  delay(500);
  moveForward();
}

void turnLeft()
{
  motorSet = "LEFT";
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
   motor1.setSpeed(speedSet+MAX_SPEED_OFFSET);
  delay(500);
  motorSet= "FORWARD";
 moveForward();
  
}

void turnAround()
{
  motorSet="TURNAROUND";
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor2.setSpeed(speedSet+ MAX_SPEED_OFFSET);

  delay(700);
 moveForward();
}
