
#include <Adafruit_NeoPixel.h>
#define NUM_PIXELS 21
#define NEOPIXEL_PIN D7
//Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);
Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW | NEO_KHZ800);

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
//#include <WiFiClient.h> 
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
int16_t gx, gy, gz; //Range: +- 2000

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO
int imuSdaPin=D3;
int imuSclPin=D5;

#define min(X, Y) (((X)<(Y))?(X):(Y))

//Parameters for Gradual display
byte gradColours[NUM_PIXELS][4];
int gHead = 0;

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

  //Neopixel section
  neopix.begin();
  //GradualColour sections
  gHead=0;
  setupGradPixels(NUM_PIXELS, 0x00, 0x00, 0x00);

}

int normalised = 4000/256; //Gyro range / RGB
void loop() {
// Check if a client has connected
  server.handleClient();

  /* Start GradualColour section */
  byte r, g, b;
  readImu();
  r = (ax+2000) / normalised;
  g = (ay+2000) / normalised;
  b = (az+2000) / normalised;
  setGradPixel(gHead, r, g, b);
  Serial.printf("%d: %d %d %d \n", gHead, r, g, b);
  gradColorCycle();
  // End GradualColour section */
  neopix.show();
  delay(200);

}

//-----Neopixel section
void gradColorCycle()
{
 int j = gHead;
  for(int i=0; i<NUM_PIXELS; i++, j++)
  {//i=neopixel position, j=colour to assign to that pixel from gradColours
    j = j % NUM_PIXELS;
    setColor(i,gradColours[j][0], gradColours[j][1], gradColours[j][2]);
  }
  gHead = abs(gHead+1) % NUM_PIXELS;
}


void setupGradPixels(int numpixels, byte r, byte g, byte b)
{
  for(int i=0; i<numpixels; i++)
  {
    setGradPixel(i, r, g, b);
  }
}

void setGradPixel(int pos, byte r, byte g, byte b)
{
  gradColours[pos][0] = r;
  gradColours[pos][1] = g;
  gradColours[pos][2] = b;
}

//set a single pixel
void setColor(int pos, byte red, byte green, byte blue) 
{
  byte white = 0;
  if(red && green && blue)
  {
    white = min(red, green);
    white = min(blue, white);
    red = red - white;
    green = green - white;
    blue = blue - white;
  }
  neopix.setPixelColor(pos, neopix.Color(red, green, blue, white));
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
    String reply = "<script language='Javascript'> ";
    reply += "var stopLoop = true; ";
    reply += "var timeoutId=0; ";

    reply += "function callReadImu() ";
    reply += "{ // alert('In callReadImu...'); ";
    reply += "  var xhttp = new XMLHttpRequest(); ";
    reply += "  xhttp.onreadystatechange = writeImu; ";
    reply += "  xhttp.open('GET', 'http://192.168.4.1/readImu', true); ";
    reply += "  xhttp.send(); } ";

    reply += "function writeImu() ";
    reply += "{ if (this.readyState == 4 && this.status == 200) ";
    reply += "    {    var result = this.responseText; ";
    reply += "        var current = document.getElementById('data').innerHTML; ";
    reply += "        document.getElementById('data').innerHTML = '<pre>' + result + '</pre>' + current;   } }";
  
    reply += "function callStart() ";
    reply += "{   stopLoop = false; ";
    reply += "  for(i=0; i<100 && !stopLoop; i++) ";
    reply += "  {  callReadImu();  } }";

    reply += "function callStop() ";
    reply += "{  stopLoop = true;";
    reply += "  window.clearTimeout(timeoutId); } ";
 
    reply += "</script> ";
    reply += "    <body>";
    reply += "  <b>Wemos Wifi Server and IMU test</b> ";
    reply += "  <p><a href='javascript:callStart()'>Start IMU</a> ";
    reply += "  <p><a href='javascript:callStop()'>Stop IMU</a> ";
    reply += "  <div name='data' id='data'></div>";
    reply += "</body> ";
    
  server.send(200, MIME_HTML, reply);
  readImu();
}

void handleReadImu() 
{
  String reading = readImu();
  server.send(200, MIME_TEXT, reading);
}


