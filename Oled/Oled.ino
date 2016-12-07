#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h" //WIFI logo

// Initialize the OLED display using Wire library
//0.96" i2c OLED , 128x64
SSD1306  display(0x3c, D3, D4); //D3=SDA, D5=SCL
//CLK always on D5
//SDA can be D3, D2

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  drawWifi();
  drawOledText("Wemos Cathy-02");
  display.display();

}

void loop() 
{
  // put your main code here, to run repeatedly:

}

//-------------------------------
void drawOledText(String text) {
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 48, text);
}

void drawWifi() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}
