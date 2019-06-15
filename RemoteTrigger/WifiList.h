#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//SSID and Password of your WiFi router
#define WIFI_SIZE 4
#define WIFI_MAX_RETRY 6
#define WIFI_CONN_INT 1000 //ms interval before reconnect to WIFI AP
IPAddress AP_IP(10,1,1,100);
IPAddress AP_GW(10,1,1,1);
IPAddress AP_SUBNET(255,255,255,0);

const char wifi_ssid[WIFI_SIZE][20] =
{
  "Miruvor",
  "Minhas Capital",
  "The Prototyping Lab",
  "FSworkshop"
};

const char wifi_pwd[WIFI_SIZE][20] =
{
   "91229334",
   "Lunch@12",
   "OMG188969",
   "12345678"
};

//=== If no WIFI AP found, setup Wemos as an AP =============================================
/* Set these to your desired credentials. */
/* You can remove the password parameter if you want the AP to be open. */
String AP_SSID = "";
const char *AP_PWD ="12345678";
