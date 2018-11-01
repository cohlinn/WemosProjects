
int sensorPin = A0;
int value;
int lastvalue=0;
void setup() {

  Serial.begin(115200);
  
}

void loop() {
  // get value from IR photodiode (0 - 1023)
  value = analogRead(sensorPin);
    Serial.println(value);
  delay(500); //if you want a slower reaction, add delay here.
}

