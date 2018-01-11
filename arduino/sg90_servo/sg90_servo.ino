/*
Into Robotics
*/
 
#include <Servo.h>  //add '<' and '>' before and after servo.h
 
int servoPin = 2;
 
Servo servo;  
 

void setup()
{
  Serial.begin(9600);  
  servo.attach(servoPin);
  servo.write(70);      // Turn SG90 servo Left to 45 degrees
  delay(1000);
  
}
 
 
void loop()
{

}
