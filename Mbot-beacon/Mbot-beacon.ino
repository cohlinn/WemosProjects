#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include "wifilist.h"

#include <NewPing.h>
#include <Adafruit_NeoPixel.h>

//https://github.com/adafruit/Adafruit_NeoPixel v1.1.8
//https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home v1.9.1

#define TRIGGER_PIN  D6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define BEACON_NEO_TOTAL 12
#define NEOPIXEL_PIN D1
Adafruit_NeoPixel strip(BEACON_NEO_TOTAL, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);

//int sonic_VCC = D5;
//int sonic_GND = D8;

int BEACON_RESET = D8;

int BEACON_DIST = 10; //max 10 cm to trigger
int BEACON_TIME = 5000; //min to remain 5 sec
int BEACON_INTERVAL = 500;
byte BEACON_COL [BEACON_NEO_TOTAL][3] = 
  {//R, G, B
    {0x50,0x00,0x00}, //NOT active
    {0x00,0x00,0x50}, //START = 0
    {0x50,0x50,0x00}, //500 ms
    {0x50,0x50,0x00}, //1000
    {0x50,0x50,0x00}, //1500
    {0x50,0x50,0x00}, //2000
    {0x50,0x50,0x00}, //2500
    {0x50,0x50,0x00}, //3000
    {0x50,0x50,0x00}, //3500
    {0x50,0x50,0x00}, //4000
    {0x50,0x50,0x00}, //4500
    {0x00,0x50,0x00}  //5000
  };
byte BEACON_OFF[3] = {0x00,0x00,0x00};

//readings values
int sonar_value = 0;
int beacon_state = 0; //0 = not triggered, 1 = triggered, but less than 5 secs, 2 = completed beacon
int beacon_timer = -1; // -1 = not started
int beacon_active = 0;
//---------------------------------------------------------------

int maxWifiRetry = BEACON_NEO_TOTAL * 3;
//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  Serial.println("Starting up ....");
  strip.begin();
//  pinMode(sonic_VCC, OUTPUT);
//  pinMode(sonic_GND, OUTPUT);
  pinMode(BEACON_RESET, INPUT);
//  digitalWrite(sonic_VCC, HIGH);
//  digitalWrite(sonic_GND, LOW);
  digitalWrite(BEACON_RESET, LOW);

  for(int i=0; i<WIFI_SIZE; i++)
  {
    if(wifiConnect(i)) //can connect to AP
    {
        //If connection successful show IP address in serial monitor
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(wifi_ssid[i]);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  //IP address assigned to your ESP

      break;
    }
      //If connection not successful show SSID
      Serial.println("");
      Serial.print("Faild to connect to ");
      Serial.println(wifi_ssid[i]);
  }

 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  yield();
  server.handleClient();          //Handle client requests

  int isReset = digitalRead(BEACON_RESET);
  Serial.print("\nBEACON_RESET: ");
  Serial.println(isReset);
  if(isReset == HIGH)
  {
    beacon_reset();
  }
  
  dist();
  if(sonar_value <= BEACON_DIST) //MBot within range
  {
    if(beacon_state == 0) //beacon not tiggered yet
    {
      beacon_state = 1;
      beacon_timer = 0;
    }
    else if(beacon_state == 1) //beacon in triggered state
    {
      beacon_timer = beacon_timer + BEACON_INTERVAL;
      if(beacon_timer >= BEACON_TIME)
        beacon_state = 2;
    }
  }
  else if(beacon_state!=2) //MBot outside range
  {
    beacon_reset();
  }
  Serial.print("beacon_state: ");
  Serial.println(beacon_state);
  Serial.print("beacon_timer: ");
  Serial.println(beacon_timer);

  neopix();
  
  
  delay(BEACON_INTERVAL);
}

boolean wifiConnect(int wifiAP)
{
    //WIFI code
  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  WiFi.begin(wifi_ssid[wifiAP], wifi_pwd[wifiAP]);     //Connect to your WiFi router

  // Wait for connection
  int pos=0;
  byte wifiblink[2][3]={{0x50,0x00,0x50} , {0x50,0x50,0x00}};
  while (WiFi.status() != WL_CONNECTED && pos < maxWifiRetry) {
    for(int i=0; i<=(pos%BEACON_NEO_TOTAL); i++)
    {
      setColor(i, wifiblink[i%2][0],wifiblink[i%2][1],wifiblink[i%2][2]);  //GRB
    }
    for(int i=(pos%BEACON_NEO_TOTAL)+1; i<BEACON_NEO_TOTAL; i++)
    {
      setColor(i, 0,0,0);  //GRB
    }
    strip.show();
    delay(BEACON_INTERVAL);
    Serial.print(".");
    pos++;
    //pos=(pos) % BEACON_NEO_TOTAL;
    
  }
  return (WiFi.status() == WL_CONNECTED);
}
//============================================================
void neopix()
{
  //Neopixel logic
  //Beacon not triggered = NEO 1 x red
  //Beacon 1st triggered = NEO 2 x blue
  //Beacon 2-10 wait = NEO 3 - 11 x yellow (9x500 ms = 4.5 ms)
  //Beacon wait 5 sec or more = NEO 12 x green  
  int pos = 0;
  if(beacon_state == 0) //not triggered
  {
    //setColor(pos,BEACON_COL[pos][0], BEACON_COL[pos][1], BEACON_COL[pos][2]);  //RGB
    neopix_setColor(0, 0);
 /*   setColor(pos,BEACON_COL[pos][1], BEACON_COL[pos][0], BEACON_COL[pos][2]);  //GRB
    for(int i=1; i<BEACON_NEO_TOTAL; i++)
    {
      setColor(i,BEACON_OFF[0], BEACON_OFF[1], BEACON_OFF[2]);
    }*/
  }
  else if(beacon_state == 1) //triggered, INPG
  {
    int OFFSET = 1;
    if(beacon_timer == 0) //1st time
    {
      int maxPos = 0 + OFFSET;
      neopix_setColor(pos, maxPos);
      /*
      for(pos=0; pos <=maxPos; pos++) //Turn on
      {
        setColor(pos,BEACON_COL[pos][1], BEACON_COL[pos][0], BEACON_COL[pos][2]);  //GRB
      }
      for(int i=maxPos+1; i<BEACON_NEO_TOTAL; i++) //Turn off
      {
        setColor(i,BEACON_OFF[0], BEACON_OFF[1], BEACON_OFF[2]);
      } */     
    }
    else //beacon_timer > 0
    {
      int maxPos = beacon_timer/BEACON_INTERVAL + OFFSET;
      neopix_setColor(pos, maxPos);
    }
  }
  else if(beacon_state == 2) //Completed
  {
    if(beacon_active == 0)
    {
      for(pos=0; pos<BEACON_NEO_TOTAL; pos++) //Turn on all
      {
        setColor(pos,BEACON_COL[pos][1], BEACON_COL[pos][0], BEACON_COL[pos][2]);  //GRB
      }
      strip.show();
      beacon_active = 1;
    }
    delay(BEACON_INTERVAL);
    colorWipe(strip.Color(0x50, 0, 0), 50);  // Green
    colorOff();
  }
  strip.show();
}

void neopix_setColor(int pos, int maxPos)
{
  Serial.print("pos: ");
  Serial.print(pos);
  Serial.print("\t maxPos: ");
  Serial.println(maxPos);
  
  for(pos=0; pos <=maxPos; pos++) //Turn on
  {
    setColor(pos,BEACON_COL[pos][1], BEACON_COL[pos][0], BEACON_COL[pos][2]);  //GRB
  }
  for(int i=maxPos+1; i<BEACON_NEO_TOTAL; i++) //Turn off
  {
    setColor(i,BEACON_OFF[0], BEACON_OFF[1], BEACON_OFF[2]);
  }      
}

void setColor(int pos, byte red, byte green, byte blue) 
{
  byte white = 0;
  strip.setPixelColor(pos, strip.Color(red, green, blue));
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Off all LEDs
void colorOff() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

//--------------------------------
int sonar_prev=0;
int sonar_zeroes=0;
void dist()
{
  sonar_prev=sonar_value;
  sonar_value = sonar.ping_cm();
  if(sonar_value == 0)
  {
    sonar_zeroes++;
    if(sonar_zeroes < 2)
      sonar_value = sonar_prev;
    else
      sonar_value = 100;
  }
  else sonar_zeroes = 0;
  Serial.print("Ping: ");
  Serial.print(sonar_value); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println(" cm");  
}

//reset beacon
/*
 * set beacon_state to 0 //not triggered
 */
void beacon_reset()
{
  beacon_state = 0;
  beacon_timer = -1;
  beacon_active = 0;
}

//-----------------------------------------------------------
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot()
{
  Serial.println("You called root page");
  char buf[4];
  //beacon_state = 0 = not triggered, 1 = triggered, but less than 5 secs, 2 = completed beacon
  switch(beacon_state)
  {
    case 0: strcpy(buf,"FOFF");
    case 1: strcpy(buf,"INPG");
    case 2: strcpy(buf,"ACTD");
  }
  server.send(200, "text/html", buf); //Send web page
}


