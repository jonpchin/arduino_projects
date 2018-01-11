#include "credentials.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Servo.h>  //add '<' and '>' before and after servo.h

int servoPin = 4;
Servo servo;  
ESP8266WebServer server(80);

/* Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */

void selectServo(){
     // Get servo from command

  servoPin = 4;
}


// Turns base servo by a certain degree
void turnBase() {
    if(server.arg("degrees") != ""){
        //Parameter found
        int result = (String(server.arg("degrees"))).toInt();

        if(result >= 0 && result <= 180){
            servo.attach(servoPin);
            servo.write(result);      // Turn SG90 servo Left to param degrees
            server.send(200, "text/html", "<h1>Servo is turning " + server.arg("degrees") +"</h1>");
        }
        else
        {
            server.send(200, "text/html", "<h1>Invalid degrees of " + String(result) +"</h1>");
        }
        
    }else
    {
        server.send(200, "text/html", "<h1>No degree parameter found.</h1>");
    }  
}

void indexHere(){
     server.send(200, "text/html", "<h1>Index reached</h1>");
}

void stopServo(){
     server.send(200, "text/html", "<h1>Servo is stopped</h1>");
    servo.detach();
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
  
    server.on("/selectservo", selectServo);
    server.on("/turnbase", turnBase);
    server.on("/stopservo", stopServo);
    server.on("/index", indexHere);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

