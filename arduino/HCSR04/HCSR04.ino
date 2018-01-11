// Jonathan Chin
// 9/9/2017
// Uses HC-SR04 sonar to detect distance in inches and centimeters
// and display it on a 1602A QAPASS LED board. A potentiometer is
// used to control the LED brightness

#include <LiquidCrystal.h> // includes the LiquidCrystal Library

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);  
const int trigPin = 9;
const int echoPin = 8;

void setup() {
    lcd.begin(16,2);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600); // Starts the serial communication
}
void loop() {
    
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    long duration = pulseIn(echoPin, HIGH);
    
    // Calculating the distance
    double distanceCm = duration/58.0;
    double distanceInch = duration/148.0;
	
    // Prints the distance on the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    Serial.print("Distance (in): ");
    Serial.println(distanceInch);
    lcd.setCursor(0,0); 
    lcd.print("Dist cm = ");
    lcd.print(distanceCm);
    lcd.setCursor(0,1); 
    lcd.print("Dist in = ");
    lcd.print(distanceInch);
    delay(200);
}
