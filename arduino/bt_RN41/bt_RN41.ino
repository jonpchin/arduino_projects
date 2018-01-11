#include <SoftwareSerial.h>  

// To do wireless update over bluetooth, connect to robot with bluetooth, set baud rate to 19200,
// then upload code and finally do a hardware reset(can be triggered soely by software)

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3

#define pwm_2 5
#define pwm_1 7
#define dir_2 4
#define dir_1 6

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
    //Serial.begin(9600);  // Begin the serial monitor at 9600bps
    bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
    bluetooth.print("$");  // Print three times individually
    bluetooth.print("$");
    bluetooth.print("$");  // Enter command mode
    delay(100);  // Short delay, wait for the Mate to send back CMD
    bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
    // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
    bluetooth.begin(9600);  // Start bluetooth serial at 9600

    pinMode(pwm_2, OUTPUT);
    pinMode(pwm_1, OUTPUT);
    pinMode(dir_1, OUTPUT);
    pinMode(dir_2, OUTPUT);
}

void loop() {
     if(bluetooth.available())  // If the bluetooth sent any characters
     {
        // Send any characters the bluetooth prints to the serial monitor
        char message = (char)bluetooth.read();

        // TODO: Mini tank uses range 0-1023 instead of -255 and 255
        if(message == 'U'){ //UP
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_1, 255);
            analogWrite(dir_2, 255);
        }else if(message == 'S'){ //STOP
            digitalWrite(pwm_1,LOW);
            digitalWrite(pwm_2,LOW);
        }else if(message == 'L'){ //LEFT SPIN
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_1, 255);
            analogWrite(dir_2,-255);
        }
        else if(message == 'R'){ // RIGHT SPIN
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_1,-255);
            analogWrite(dir_2,255);
        } else if(message == 'B'){ //Backwards
            //Serial.print("BACKWARDS");
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_1, -255);
            analogWrite(dir_2, -255);
        }else if(message == '1'){ // top left
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,LOW);
            analogWrite(dir_1, 255);
        }else if(message == '2'){ // top right
            digitalWrite(pwm_1,LOW);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_2,255);
        }else if(message == '3'){ // bottom left
            digitalWrite(pwm_1,HIGH);
            digitalWrite(pwm_2,LOW);
            analogWrite(dir_1,-255);
        }else if(message == '4'){ // bottom right
            digitalWrite(pwm_1,LOW);
            digitalWrite(pwm_2,HIGH);
            analogWrite(dir_2,-255);
        }
    }
}

