#include "credentials.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Servo.h>

// Pin numbers for each servo
#define CLAW  5
#define LEFT  2
#define RIGHT 0
#define BASE  4

// Keep track of servo in servos list
#define SERVO_CLAW  0
#define SERVO_LEFT  1
#define SERVO_RIGHT 2
#define SERVO_BASE  3

Servo servoClaw;
Servo servoLeft;
Servo servoRight;
Servo servoBase;  

int servoPin = BASE;
Servo servos[4] = {servoClaw, servoLeft, servoRight, servoBase};
Servo selectedServo = servos[SERVO_BASE]; //default servo pin is base if none is specified

ESP8266WebServer server(80);

/* Go to 192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
// 192.168.1.4/attachservo?servoname=claw
void attachServo(){

    detachServo();
    
    if(server.arg("servoname") == "claw"){
        servoClaw.attach(CLAW);
        selectedServo = servos[SERVO_CLAW];
        getServoAngle();
    }else if(server.arg("servoname") == "left"){
        servoLeft.attach(LEFT);
        selectedServo = servos[SERVO_LEFT];
        getServoAngle();
    }else if(server.arg("servoname") == "right"){
        servoRight.attach(RIGHT);
        selectedServo = servos[SERVO_RIGHT];
        getServoAngle();
    }else if(server.arg("servoname") == "base"){
        servoBase.attach(BASE);
        selectedServo = servos[SERVO_BASE];
        getServoAngle();
    }else{
        server.send(200, "text/html", "<h1>No valid parameter for attachServo.</h1>");
    }
}

String getServoPinName(int pin){
    if(pin == CLAW){
        return "claw";
    }else if(pin == LEFT){
        return "left";
    }else if(pin == RIGHT){
        return "right";
    }else if(pin == BASE){
        return "base";
    }else{
        return ""; // Reserved for motor?
    }
}

// Turns base servo by a certain degree
// 192.168.1.4/turnservo?degrees=15
void turnServo(){
    if(server.arg("degrees") != ""){
        //Parameter found
        int result = (String(server.arg("degrees"))).toInt();

        if(result >= 0 && result <= 180){
            selectedServo.write(result);
            server.send(200, "text/html", "<h1>" + getServoPinName(servoPin) +" servo is turning " + server.arg("degrees") +"</h1>");
        }
        else
        {
            server.send(200, "text/html", "<h1>Invalid degrees of " + String(result) +"</h1>");
        }
        
    }else
    {
         server.send(200, "text/html", "<h1>"+ getServoPinName(servoPin) + " servo is missing degrees.</h1>");
    } 
}

int getServoAngle(){
    server.send(200, "text/html", String(selectedServo.read()));
}

void detachServo(){
    int i;
    int total = sizeof(servos)/sizeof(servos[0]);
    for(i=0; i<total; ++i){
        servos[i].detach(); 
    }
}
void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.print("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // If no servo is attached then this will allow the motor to be enabled
    server.on("/attachservo", attachServo);
    server.on("/turnservo", turnServo);
    server.on("/detachservo", detachServo);
    server.on("/getservoangle", getServoAngle);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

