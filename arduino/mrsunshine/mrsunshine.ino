#include <avr/sleep.h>
#include <SoftwareSerial.h>

SoftwareSerial bt(8, 9); // RX (8 has to be RX), TX

void toggle_sleep()
{
    if (digitalRead(2) == LOW) { //wakeing up
        // cancel sleep as a precaution
        sleep_disable();
        detachInterrupt (0);

    } else if (digitalRead(2) == HIGH) { // going to sleep

        // disable ADC
        ADCSRA = 0;

        set_sleep_mode (SLEEP_MODE_PWR_DOWN);
        sleep_enable();

        // Do not interrupt before we go to sleep, or the
        // ISR will detach interrupts and we won't wake.
        noInterrupts ();

        // will be called when pin D2 goes low
        attachInterrupt (0, toggle_sleep, CHANGE);
        EIFR = bit (INTF0);  // clear flag for interrupt 0

        // turn off brown-out enable in software
        // BODS must be set to one and BODSE must be set to zero within four clock cycles
        MCUCR = bit (BODS) | bit (BODSE);
        // The BODS bit is automatically cleared after three clock cycles
        MCUCR = bit (BODS);

        // We are guaranteed that the sleep_cpu call will be done
        // as the processor executes the next instruction after
        // interrupts are turned on.
        interrupts ();  // one cycle
        sleep_cpu ();   // one cycle
    }
}

void setup ()
{
    //need this to be the same as the Soft Serial
    Serial.begin(115200);
    bt.begin(115200);
    Serial.println("RN4020 testing");
    delay(1000);
    bt.print("SF,2\n");
    delay(1000);
    getResponse();
    bt.print("R,1\n");
}

void getResponse(){
    // Feed any data from bluetooth to Terminal.
    if (bt.available()){
        Serial.print((char)bt.read());
        /*
        char letter = bt.read();
        if(letter == 'U'){
            Serial.write(letter);
        }else if(letter == 'D'){
            Serial.write(letter);
        }else if(letter == 'L'){
            Serial.write(letter);
        }else if(letter == 'R'){
            Serial.write(letter);
        }
        */
    }
       
     
      // Feed all data from termial to bluetooth
    if (Serial.available()){
        bt.print((char)Serial.read());
    }
}

void loop ()
{
    // Feed any data from bluetooth to Terminal.
    if (bt.available()){
        Serial.print((char)bt.read());
        /*
        char letter = bt.read();
        if(letter == 'U'){
            Serial.write(letter);
        }else if(letter == 'D'){
            Serial.write(letter);
        }else if(letter == 'L'){
            Serial.write(letter);
        }else if(letter == 'R'){
            Serial.write(letter);
        }
        */
    }
       
     
      // Feed all data from termial to bluetooth
    if (Serial.available()){
        bt.print((char)Serial.read());
    }
        

    //attachInterrupt (0, toggle_sleep, CHANGE);
}
