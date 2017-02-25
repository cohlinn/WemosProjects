int mA1 = D1;
int mA2 = D2;
int mB1 = D5;
int mB2 = D6;
int mSpeed = D3;

void setup() 
{
  Serial.begin(115200);
  //Setup pins for motor control
  pinMode(mA1, OUTPUT);
  pinMode(mA2, OUTPUT);
  pinMode(mB1, OUTPUT);
  pinMode(mB2, OUTPUT);
  digitalWrite(mA1, LOW);
  digitalWrite(mA2, LOW);
  digitalWrite(mB1, LOW);
  digitalWrite(mB2, LOW);
  //set speed
  analogWrite(mSpeed, 700);
}

void loop() {
  int interval = 2000;
  moveFwd();
  delay(interval);
  moveStop();
  delay(interval);
  moveBwd();
  delay(interval);
  moveStop();
  delay(interval);
  moveLeft();
  delay(interval);
  moveStop();
  delay(interval);
  moveRight();
  delay(interval);
  moveStop();
  delay(interval);
}

void moveFwd()
{
  digitalWrite(mA1, HIGH);
  digitalWrite(mA2, LOW);
  digitalWrite(mB1, HIGH);
  digitalWrite(mB2, LOW);
}

void moveBwd()
{
  digitalWrite(mA1, LOW);
  digitalWrite(mA2, HIGH);
  digitalWrite(mB1, LOW);
  digitalWrite(mB2, HIGH);
}

void moveRight()
{
  digitalWrite(mA1, HIGH);
  digitalWrite(mA2, LOW);
  digitalWrite(mB1, LOW);
  digitalWrite(mB2, HIGH);
}

void moveLeft()
{
  digitalWrite(mA1, LOW);
  digitalWrite(mA2, HIGH);
  digitalWrite(mB1, HIGH);
  digitalWrite(mB2, LOW);
}

void moveStop()
{
  digitalWrite(mA1, LOW);
  digitalWrite(mA2, LOW);
  digitalWrite(mB1, LOW);
  digitalWrite(mB2, LOW);
}
