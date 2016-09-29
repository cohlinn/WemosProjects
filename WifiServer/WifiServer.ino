
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

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

// IMU
// class default I2C address is 0x68, // specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69   //MPU6050 accelgyro(0x69); // <-- use for AD0 high
MPU6050 imu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO
int imuSdaPin=D3;
int imuSclPin=D5;

void setup()
{
  Serial.begin(115200);
  //Setup Wifi AP
  setupWiFi();
  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.on("/readImu", handleReadImu);
  server.begin();
  Serial.println("HTTP server started");

  setupImu();
  //Start Webserver
  //server.begin();
}


void loop() {
// Check if a client has connected
  server.handleClient();
}

//----------------------------------------

void setupImu()
{
  Wire.begin(imuSdaPin,imuSclPin);

   // initialize device
   Serial.println("Initializing I2C devices...");
   imu.initialize();
   // verify connection
   Serial.println("Testing device connections...");
   Serial.println(imu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");


}

String readImu()
{
  String result;
  // read raw accel/gyro measurements from device
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // these methods (and a few others) are also available
    //imu.getAcceleration(&ax, &ay, &az);
    //imu.getRotation(&gx, &gy, &gz);
 
  // display tab-separated accel/gyro x/y/z values
  result = "a/g:\t";
  result += (millis()/1000);
  result += ":";
  result += (millis()%1000);
  result += "\t";
  result += (ax);
  result += "\t";
  result += (ay);
  result += "\t";
  result += (az);
  result += "\t";
  result += (gx);
  result += "\t";
  result += (gy);
  result += "\t";
  result += (gz);
  Serial.println(result);
  return result;
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
    "<b>You are connected to "+AP_NameString+"</b>"+
    "<p><a href='/start'>Start IMU</a></p>");
}

void handleStart() {
    String reply = "<script language='Javascript'> \r\n";
    reply += "var stopLoop = true; var timeoutId=0; \r\n";
    reply += "function callReadImu() { alert('In callReadImu...'); \r\n";
    reply += "  var xhttp = new XMLHttpRequest(); \r\n";
    reply += "  http.onreadystatechange = function() \r\n";
    reply += "  { if (this.readyState == 4 && this.status == 200) \r\n";
    reply += "    {   var result = this.responseText; \r\n";
    reply += "        var current = document.getElementById('data').innerHTML; \r\n";
    reply += "        document.getElementById('data').innerHTML = '<pre>' + result + '</pre>' + current; } \r\n";
    reply += "  }; \r\n";
    reply += "  xhttp.open('GET', '/readImu', true); \r\n";
    reply += "  xhttp.send(); \r\n";
    reply += "} \r\n";
    reply += "function callStart() { \r\n";
    reply += "  stopLoop = false; \r\n";
    reply += "  while(!stopLoop) { timeoutId = window.setTimeout(callReadImu, 1000); } \r\n";
    reply += "} \r\n";
    reply += "function callStop() { stopLoop = true; window.clearTimeout(timeoutId); } \r\n";
    reply += "window.onload=callStart;  \r\n";
    reply += "</script> \r\n";
    reply += "<body><b>You are connected to "+AP_NameString+"</b> \r\n"
    + "<p><a href='javascript:callStart()'>Start IMU</a> \r\n"
    + "<p><a href='javascript:callStop()'>Stop IMU</a> \r\n"
    + "<div name='data' id=\"data\"></div></body> \r\n";
    
  server.send(200, MIME_HTML, reply);
  readImu();
}

void handleReadImu() 
{
  String reading = readImu();
  server.send(200, MIME_TEXT, reading);
}


