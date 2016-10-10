#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h" //WIFI logo
#include <NewPing.h>

#define TRIGGER_PIN  D6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D3, D5); //D3=SDA, D5=SCL
int MOTORPIN2 = D2;
int MOTORPIN1 = D1;

int UPPIN = D8;
int value = 0;
int stopflag = LOW;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
    // Initialising the UI will init the display too.

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  drawWifi();
  display.display();
  
   pinMode(UPPIN, INPUT);


   //Motor section
   pinMode(MOTORPIN1, OUTPUT);
   pinMode(MOTORPIN2, OUTPUT);
   digitalWrite(MOTORPIN1, LOW);
   digitalWrite(MOTORPIN2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  yield();

  value = sonar.ping_cm();

  Serial.print("Ping: ");
  Serial.print(value); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println(" cm");

  display.clear();
  drawOledText(value);
  display.display();

  //motor section
  int stopflag = digitalRead(UPPIN);
  Serial.println("Button = "+stopflag);
  if(stopflag == LOW)
  {
    if(value <30)
      moveDown(5);
    else moveUp(5);
  } 
  else
  {
     digitalWrite(MOTORPIN1, LOW);
     digitalWrite(MOTORPIN2, LOW);
  }
  
   //delay(1000);
}

void moveDown(int value)
{
   digitalWrite(MOTORPIN1, HIGH);
   digitalWrite(MOTORPIN2, LOW);
   delay(value*100);
   digitalWrite(MOTORPIN1, LOW);
   digitalWrite(MOTORPIN2, LOW);
}

void moveUp(int value)
{
   digitalWrite(MOTORPIN1, LOW);
   digitalWrite(MOTORPIN2, HIGH);
   delay(value*100);
   digitalWrite(MOTORPIN1, LOW);
   digitalWrite(MOTORPIN2, LOW);
}

void drawOledText(int value) {
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Wemos Cathy-01");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 24, "Distance:");
    display.setFont(ArialMT_Plain_16);
    display.drawString(30, 35, String(value) + " cm");
}

void drawWifi() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}
