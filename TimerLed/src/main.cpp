#include <Arduino.h>
#include <ESP8266WiFi.h>
#define LED_PIN D3
#define BUTTON_PIN D8

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started.");
  digitalWrite(BUTTON_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

int LED_ON = 0;
void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) 
  {
    if(LED_ON == 0)
    {
      Serial.println("Turn on LED.");
      digitalWrite(LED_PIN, HIGH);
      LED_ON = 60*1; //1 minute
    }
    else if(LED_ON > 0)
    {
      LED_ON = 0;
      digitalWrite(LED_PIN, LOW);
      Serial.println("Turn off LED.");
    }
  }  else if(LED_ON > 0)
  {
    LED_ON--;
    if(LED_ON == 0)
    {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Turn off LED.");
    }
  }

  delay(1000);
}