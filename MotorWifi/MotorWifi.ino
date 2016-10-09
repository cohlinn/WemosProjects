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

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif


/* Create a WiFi access point and provide a web server on it. */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "CATHY";
const char WiFiAPPSK[] = "mysecret";
ESP8266WebServer server(80);
String AP_NameString;

int mPin1 = D1;
int mPin2 = D2;

void setup() 
{
  Serial.begin(115200);
  //Setup pins for motor control
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, LOW);

    // Initialising the UI will init the display too.

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  drawWifi();
  display.display();
  
    //Setup Wifi AP
  setupWiFi();
  server.on("/", handleRoot);
  server.on("/up", handleUp);
  server.on("/down", handleDown);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
// Check if a client has connected
  server.handleClient();

  int value = sonar.ping_cm();

  Serial.print("Ping: ");
  Serial.print(value); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println(" cm");

  display.clear();
  drawOledText(value);
  display.display();

  //motor section
  if(value <10)
  {
    value = sonar.ping_cm();
    if(value <10) moveStop(5);
  }

}


void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  AP_NameString = WiFiSSID + macID;

  Serial.println("Configuring access point "+AP_NameString);
  char APchar[AP_NameString.length()+1];
  AP_NameString.toCharArray(APchar, AP_NameString.length());
  
  WiFi.softAP(APchar, WiFiAPPSK);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}


/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.
 * Webserver handling.
 */
const String MIME_HTML = "text/html";
const String MIME_TEXT = "text/plain";
void handleRoot() {
  server.send(200, MIME_HTML, 
    "<b>You are connected to "+AP_NameString+"</b>"
    + "<p><a href='/up'><button type='button' value='Move Up'>Move Up</button></a></p>"
    + "<p><a href='/down'><button type='button' value='Move Down'>Move Down</button></a></p>"
    + "<p><a href='/stop'><button type='button' value='Stop'>Stop</button></a></p>");
}

void handleUp() 
{
  //return no content
  server.send(204, MIME_TEXT, "");
  Serial.println("Move 1");
  digitalWrite(mPin1, HIGH);
  digitalWrite(mPin2, LOW);
}

void handleDown() 
{
  //return no content
  server.send(204, MIME_TEXT, "");
  Serial.println("Move 2");
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, HIGH);
}

void handleStop() 
{
  //return no content
  server.send(204, MIME_TEXT, "");
  Serial.println("Stop");
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, LOW);
}

void moveStop(int value)
{
   digitalWrite(mPin1, LOW);
   digitalWrite(mPin2, LOW);
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

