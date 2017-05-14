#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 120
#define NEOPIXEL_PIN D1
Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);
//Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW | NEO_KHZ800);

//TO BE MODIFIED according to need
//Parameters for Solid display
#define SOLID_LIST_LEN 7
byte solidColours[SOLID_LIST_LEN][3] = 
  { //G R B
    {0x00,0x00,0x00}, //off
    {0x00,0x00,0xDF}, //blue
    {0x00,0xDF,0x00}, //red
    {0xA0,0xA0,0x10}, //yellow
    {0xDF,0x00,0x10}, //green
    {0x00,0xA0,0xA5}, //purple
    {0xA0,0xA0,0xA0} //white
  };
int solidIndex=0;

#define SWITCH D5
#define BUTTON D7

boolean SW_ON = true;
boolean B_PRESS = false;

void setup() 
{
  Serial.begin(115200);
  neopix.begin();

  for(int i=0; i<NUM_PIXELS; i++)
  {
    setColor(i,solidColours[solidIndex][0], solidColours[solidIndex][1], solidColours[solidIndex][2]);
  }

  neopix.show();

  pinMode(SWITCH, INPUT);
  pinMode(BUTTON, INPUT);
}

void loop() 
{
  Serial.println(".....");  

  if(digitalRead(SWITCH) && !SW_ON) //SW=ON, previously OFF
  { //turn on
    setStrip(solidIndex);
    neopix.show();
    SW_ON = true;
  }
  else if(!digitalRead(SWITCH) && SW_ON) //switch off
  { //turn off
    setStrip(0);
    neopix.show();
    SW_ON = false;
  }
  else if(SW_ON && digitalRead(BUTTON) && !B_PRESS) //press button
  { //change colour
    solidIndex++;
    setStrip(solidIndex);
    neopix.show();
    B_PRESS = true;
  }
  else if(!digitalRead(BUTTON) && B_PRESS) //let go button
  {
    B_PRESS = false;
  }

  delay(1000);
}


void setStrip(int colour)
{
  for(int i=0; i<NUM_PIXELS; i++)
  {
    setColor(i,solidColours[colour][0], solidColours[colour][1], solidColours[colour][2]);
  }
}



//======== Common functions
//set a single pixel
void setColor(int pos, byte red, byte green, byte blue) 
{
//  byte white = 0;
//  if(red && green && blue)
//  {
//    white = min(red, green);
//    white = min(blue, white);
//    red = red - white;
//    green = green - white;
//    blue = blue - white;
//  }
//  neopix.setPixelColor(pos, neopix.Color(red, green, blue, white));
  //neopix.setPixelColor(pos, neopix.Color(red, green, blue,0));
  neopix.setPixelColor(pos, neopix.Color(red, green, blue));
}


//====== Functions for Solid Colours
/*
  Code for controling Neopixels.
  This sets the all neopixels into the same colour.
  The next time it is called, it will set all neopixels to the next colour on the list.
  The list of colours and its length is defined by the user.
  Created by Cathy Ooi, 2016-10-07.
  Released into the public domain.
*/

void setAllSameColor(int numpixels, byte *rgb)
{
  for(int i=0; i<numpixels; i++)
  {
    setColor(i, rgb[0], rgb[1], rgb[2]);
  }
}

void solidColorCycle(int numpixels, byte colorList[][3], int colorListLen)
{
  setAllSameColor(numpixels, colorList[solidIndex]);
  solidIndex = (solidIndex+1) % colorListLen;
}

