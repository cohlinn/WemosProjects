#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const unsigned long ntpRefreshIntervalMs = 60UL * 1000UL;

void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void printCurrentTime() {
  time_t now = time(nullptr);
  if (now < 1000000000) {
    Serial.println("Time not available yet.");
    return;
  }

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
  Serial.print("Current time: ");
  Serial.println(buffer);
}

void getTimeFromNtp() {
  configTime(0, 0, ntpServer1, ntpServer2);

  Serial.println("Waiting for NTP time...");
  time_t now = time(nullptr);
  while (now < 1000000000) {
    delay(500);
    now = time(nullptr);
  }

  printCurrentTime();
}

void setup() {
  Serial.begin(115200);
  Serial.print("Setup started.");

  connectToWiFi();
  Serial.print("Connected to WiFi.");
  Serial.print("Getting time from NTP server...");
  getTimeFromNtp();
}

void loop() {

  printCurrentTime();

  delay(1000);
}