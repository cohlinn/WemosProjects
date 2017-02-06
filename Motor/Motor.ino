int mPin1 = D1;
int mPin2 = D2;
int mSpeed = D3;

void setup() 
{
  Serial.begin(115200);
  //Setup pins for motor control
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, LOW);
  //set speed
  analogWrite(mSpeed, 700);
}

void loop() {
  Serial.println("Move 1");
  digitalWrite(mPin1, HIGH);
  digitalWrite(mPin2, LOW);
  delay(1000);
  Serial.println("Move 2");
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, HIGH);
  delay(1000);
  Serial.println("Stop");
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, LOW);
  delay(1000);
}
