int LED = 1;
int SENSOR_PIN = 2; //AO pin

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
}

void loop()
{
  int value = analogRead(SENSOR_PIN);
  if(value == HIGH){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}
