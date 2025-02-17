

/* Create a WiFi access point and provide a web server on it. */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "WifiList.h"

uint8_t macAddr[6];
//////////////////////
// WiFi Definitions //
//////////////////////

ESP8266WebServer server(80);
String AP_NameString;
String WEMOS_IP;

/* 
 *  360 Servo control
 *  These are driven by a PWM signal.  
 *  A pulse width of approximately 1500uS (PWM value of about 185) will cause the motor to stop.  
 *  A higher PWM value over about 195 will rotate the motor in one direction 
 *  while a lower PWM value under about 175 will cause the motor to rotate in the opposite direction.  
 *  If the PWM value drops below about 500uSec (PWM value of 62), the motor will also stop due to insufficient drive.
 *  If using the Servo.h library, as shown in the code below, a command of about 
 *  90 will cause the motor to stop. 
 *  0 will cause it turn in one direction at full speed
 *  180 will cause it to turn in the other direction at full speed.
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int servo_pin = D2; //D2 = GPIO4
int servodir = 0; //0=stop, -1=left, 1=right

void setup() {
  Serial.begin(9600);
  myservo.attach(servo_pin,500,2500);  // attaches the servo on pin 9 to the servo object
  //Setup Wifi AP
  for(int i=0; i<WIFI_SIZE; i++)
  {
    if(wifiConnect(i)) //can connect to AP
    {
        //If connection successful show IP address in serial monitor
      Serial.println("");
      Serial.println("Connected to " + String(wifi_ssid[i]));
      Serial.print("IP address: ");
      WEMOS_IP = WiFi.localIP().toString();
      Serial.println(WEMOS_IP);  //IP address assigned to your ESP

      break;
    }
    //If connection not successful show SSID
    Serial.println("");
    Serial.print("Failed to connect to ");
    Serial.println(wifi_ssid[i]);
  }
  server.on("/", handleRoot);
  server.on("/feed", handleFeed);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.begin();
  Serial.println("HTTP server started");
  ServoStop();
 }
 
void loop()
{
  yield();
  server.handleClient();
  //ServoStop();
 }

 void ServoStop()
 {
  myservo.write(90);
 }

 void ServoRight()
 {
  myservo.write(70);
 }

 void ServoLeft()
 {
  myservo.write(110);
 }

 
//=== WIFI functions ==============================================================
boolean wifiConnect(int wifiAP)
{
    //WIFI code
  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  WiFi.begin(wifi_ssid[wifiAP], wifi_pwd[wifiAP]);     //Connect to your WiFi router

  // Wait for connection
  int pos=0;
  while (WiFi.status() != WL_CONNECTED && pos < WIFI_MAX_RETRY)
  {
    delay(WIFI_CONN_INT);
    Serial.print(".");
    pos++;
  }
  return (WiFi.status() == WL_CONNECTED);
}

void handleRoot()
{
  Serial.println("Triggered http://" + WEMOS_IP);
  server.send(200, "text/html", "<HTML><HEAD><meta name='viewport' content='width=device-width, initial-scale=1'/> \
  <BASE href='http://"+WEMOS_IP+"' target='outframe'/>Fish Feeder</HEAD><BODY> \
  <p/><a href='/feed'> Feed Fish </a> \
  <p/><a href='/left'> Shift Left</a> \
  <p/><a href='/right'> Shift Right </a> \
  <p/><iframe id='outframe' name='outframe' title='Output result' width='300' height='55'/></iframe> \
  </BODY></HTML>");
}

void handleFeed()
{
  Serial.println("Triggered http://" + WEMOS_IP + "/feed");
   server.send(200, "text/plain", "Feed Trigger fired ");
   ServoLeft();
  delay(1000);
  ServoStop();
  delay(1000);
  ServoRight();
  delay(1000);
  ServoStop();

}


void handleLeft()
{
  Serial.println("Triggered http://" +WEMOS_IP + "/left");

      ServoLeft();
      delay(15);
      ServoStop();
  server.send(200, "text/plain", "Left Trigger fired ");
}


void handleRight()
{
  Serial.println("Triggered http://" +WEMOS_IP + "/right");

      ServoRight();
      delay(15);
      ServoStop();
 
  server.send(200, "text/plain", "Right Trigger fired ");
}
