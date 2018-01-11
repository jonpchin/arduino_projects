//Import the Wire Library for I2C Communication
#include <Wire.h>

#define I2C_SLAVE_ADDRESS   8 

unsigned int count = 0;

void setup() {
  //Initialize the I2C 
  Wire.begin(I2C_SLAVE_ADDRESS);
  //Serial monitor 
  Serial.begin(9600);                
  //Regiseter the event for the callback
  Wire.onRequest(i2c_callback); 
}

void loop() {
  delay(100);
}

void i2c_callback() {
  while(Serial.available()){
  	  //Send to the requested RPI
	  Wire.write((char)Serial.read()); // respond with message of 6 byte
  }
 
}

//End of the Code

