#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h" //WIFI logo

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, D3, D4); //D3=SDA, D5=SCL


int sensorPin = A0;
int value;
int lastvalue=0;
void setup() {

  Serial.begin(115200);
  
  // put your setup code here, to run once:
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  drawWifi();
  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(sensorPin);
  if(value != lastvalue)
  {
    display.clear();
    drawOledText(lastvalue, value);
    display.display();

    Serial.println(value);
  }
  lastvalue = value;
  delay(100);
}

void drawOledText(int prev, int value) {
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Wemos Cathy-01");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 24, String(prev));
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 35, String(value));
}

void drawWifi() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}
