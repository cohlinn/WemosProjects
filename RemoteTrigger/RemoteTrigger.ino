#include "WifiList.h"
ESP8266WebServer server(80); //Server on port 80
uint8_t macAddr[6];

#define TRIGGER D4
String WEMOS_IP;

void setup() 
{
  Serial.begin(115200);
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);
    //Setup Wifi and Webserver`
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
  server.on("/fire", handleFire);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() 
{
  yield();
  server.handleClient();
  delay(200);
}


void handleRoot()
{
  Serial.println("Triggered http://" + WEMOS_IP);
  digitalWrite(TRIGGER, LOW);
  server.send(200, "text/html", "<HTML><HEAD><meta name='viewport' content='width=device-width, initial-scale=1'/> <BASE href='http://"+WEMOS_IP+"' target='outframe'/>RemoteTrigger</HEAD><BODY> \
  <FORM action='/fire?' target='outframe'> \
    <INPUT Type='SUBMIT' value='--FIRE--'/>  \
    Duration <INPUT type='number' name='duration' min='10' max='3000' maxlength='8' size='5' /> \
  </FORM> \
  <iframe id='outframe' name='outframe' title='Output result' width='300' height='55'/></iframe> \
  </BODY></HTML>");
}

void handleFire()
{
  Serial.println("Triggered http://" +WEMOS_IP + "/fire?duration="+server.arg(0));
  int duration = 500; //ms by default
  String temp;
  if(server.args()>0)
  { 
    temp = server.arg(0);
    if(isDigit(temp.charAt(0)))
      duration  = temp.toInt();
    if(duration < 10)
      duration = 500;
  }
  digitalWrite(TRIGGER, HIGH);
  delay(duration);
  digitalWrite(TRIGGER, LOW);
  server.send(200, "text/plain", "Remote Trigger fired "+micros());
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
