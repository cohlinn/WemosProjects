



#include <Wire.h>
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"
//https://github.com/squix78/esp8266-oled-ssd1306
#include "images.h" //WIFI logo
#include <NewPing.h>

#define TRIGGER_PIN  D3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D4  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//// Initialize the OLED display using Wire library
//SSD1306  display(0x3c, D3, D4); //D3=SDA, D4=SCL




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
boolean APMODE = false; //false = connect to Wifi, true = setup Wifi Access Point (IP=192.168.4.1)
//config for public wifi
const char ssid[] = "PrototypingLab";
const char password[] = "xxxx";


int mR1 = D5;
int mR2 = D6;
int mL1 = D1;
int mL2 = D2;
int mSpeed = A0;
#define APPIN  D7 //HIGH=AP, LOW=WIFI

int currSpeed = 600;
String mStatus = "Status.....";
boolean autoFlag = false; //Start with stationary

void setup() 
{
  Serial.begin(115200);
  //Setup pins for motor control
  pinMode(mR1, OUTPUT);
  pinMode(mR2, OUTPUT);
  digitalWrite(mR1, LOW);
  digitalWrite(mR2, LOW);

  pinMode(mL1, OUTPUT);
  pinMode(mL2, OUTPUT);
  digitalWrite(mL1, LOW);
  digitalWrite(mL2, LOW);

  analogWrite(mSpeed, 0); //0 - 1023 speed. 0 - stop
  mStatus = "Stop";

  //Decide to connect to WIFI router or turn self into WIFI router
  pinMode(APPIN, INPUT);
  (digitalRead(APPIN)==HIGH)? APMODE=true: APMODE=false;

    // Initialising the UI will init the display too.
/*  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
    // clear the display
  display.clear();
  drawWifi();
  display.display();
 */ 
    //Setup Wifi AP
  setupWiFi();
  server.on("/", handleRoot);
  server.on("/fwd", handleFwd);
  server.on("/back", handleBack);
  server.on("/right", handleRight);
  server.on("/left", handleLeft);
  server.on("/stop", handleStop);
  server.on("/auto", handleAuto);
  server.begin();
  Serial.println("HTTP server started");
}

int dist;
#define MIN_DIST 10
void loop() {
// Check if a client has connected
  server.handleClient();
  if(autoFlag)
  {
    dist = sonar.ping_cm();
    if(dist > MIN_DIST)
      moveFwd();
    else
    {
      int start = millis();
      int lapse = 0;
      while(dist<=MIN_DIST && lapse < 10000) //find direction to move, or give up after 2 seconds
      {
        moveLeft();
        delay(100);
        dist = sonar.ping_cm();
        lapse = millis() - start;
      }
      
      
      if(dist<=MIN_DIST )
        moveStop();
      //sound trapped alarm
    }
  }

//  display.clear();
//  drawOledText(mStatus);
//  display.display();
//  Serial.println(mStatus);

}


void setupWiFi()
{
  if(APMODE)
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
  else
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

/*
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Wemos "+WiFi.localIP());
    display.setFont(ArialMT_Plain_10);
    display.display();
*/
  }
}


/* Go to http://192.168.4.1 in a web browser connected to this access point to see it.
 * Webserver handling.
 */
const String MIME_HTML = "text/html";
const String MIME_TEXT = "text/plain";
void handleRoot() {
  server.send(200, MIME_HTML, 
    "<b>You are connected to "+AP_NameString+"</b>"
    + "<p><a href='/fwd'><button type='button' value='Forward'>Forward</button></a></p>"
    + "<p><a href='/left'><button type='button' value='Left'>Left</button></a>"
    +    "<a href='/right'><button type='button' value='Right'>Right</button></a></p>"
    + "<p><a href='/back'><button type='button' value='back'>Back</button></a></p>"
    + "<p><a href='/stop'><button type='button' value='Stop'>Stop</button></a></p>"
    + "<p><a href='/auto'><button type='button' value='Auto'>Auto Move</button></a></p>");
}


void handleAuto() 
{
  //return no content
  server.send(204, MIME_TEXT, "Auto");
  Serial.println("Auto");
  autoFlag=true;
}


void handleFwd() 
{
  moveFwd();
  //return no content
  server.send(204, MIME_TEXT, "Forward");
}
void moveFwd()
{
  Serial.println("Forward");
  digitalWrite(mR1, HIGH);
  digitalWrite(mR2, LOW);
  digitalWrite(mL1, HIGH);
  digitalWrite(mL2, LOW);
  analogWrite(mSpeed, currSpeed);

}

void handleBack() 
{
  Serial.println("Back");
  digitalWrite(mR1, LOW);
  digitalWrite(mR2, HIGH);
  digitalWrite(mL1, LOW);
  digitalWrite(mL2, HIGH);
  analogWrite(mSpeed, currSpeed);
  //return no content
  server.send(204, MIME_TEXT, "");
}

void handleLeft() 
{
  moveLeft();
  //return no content
  server.send(204, MIME_TEXT, "");
}

void moveLeft()
{
  Serial.println("Left");
  digitalWrite(mR1, HIGH);
  digitalWrite(mR2, LOW);
  digitalWrite(mL1, LOW);
  digitalWrite(mL2, HIGH);
  analogWrite(mSpeed, currSpeed);  
}

void handleRight() 
{
  Serial.println("Right");
  digitalWrite(mR1, LOW);
  digitalWrite(mR2, HIGH);
  digitalWrite(mL1, HIGH);
  digitalWrite(mL2, LOW);
  analogWrite(mSpeed, currSpeed);
  //return no content
  server.send(204, MIME_TEXT, "");
}

void handleStop() 
{
  moveStop();
  //return no content
  server.send(204, MIME_TEXT, "");
}

void moveStop()
{
  autoFlag = false;
  Serial.println("Stop");
  analogWrite(mSpeed, 0);
  digitalWrite(mR1, LOW);
  digitalWrite(mR2, LOW);
  digitalWrite(mL1, LOW);
  digitalWrite(mL2, LOW); 
}


/* //Draw on screen
void drawOledText(String value) {
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Wemos "+WiFi.localIP());
    display.setFont(ArialMT_Plain_10);
//    display.drawString(0, 24, mStatus);
//    display.setFont(ArialMT_Plain_10);
//    display.drawString(30, 35, "Speed:" + currSpeed);
}

void drawWifi() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}
*/
