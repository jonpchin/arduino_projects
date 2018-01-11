// Jonathan Chin
// 9/9/2017
// Blinking LED

int ledPin = 0;

void setup()  {
	pinMode(ledPin, OUTPUT);
	for(int i=0;i<10;i++) 
	{
		digitalWrite(ledPin, HIGH);
		delay(1000);
		digitalWrite(ledPin, LOW);
		delay(1000);
	}
}

void loop() {
  
}

