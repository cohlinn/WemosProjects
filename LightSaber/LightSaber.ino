/**
 * TODO: 
 * Add web interface to change colour
 * Add Rainbow colour scheme
 * Add flickering fire effect
 */
#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 103
#define NEOPIXEL_PIN D1
Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);
//Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW | NEO_KHZ800);

//TO BE MODIFIED according to need
//Parameters for Solid display

#define SOLID_LIST_LEN 7
byte offColour[3] = {0x00, 0x00, 0x00};
byte solidColours[SOLID_LIST_LEN][3] = 
  { //G R B
    {0x1F,0x00,0x00}, //green
    {0x00,0x00,0x1F}, //blue
    {0x00,0x1F,0x00}, //red
    {0x0F,0x0F,0x00}, //yellow
    {0x0F,0x00,0x05}, //blue-green
    {0x00,0x0F,0x0F}, //purple
    {0x0F,0x0F,0x0F} //white
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
  yield();
  Serial.println(".....");  

  if(digitalRead(SWITCH) && !SW_ON) //SW=ON, previously OFF
  { //turn on
    Serial.println("Turning on");
    solidIndex++;
    setStrip(solidIndex);
    neopix.show();
    SW_ON = true;
  }
  else if(!digitalRead(SWITCH) && SW_ON) //switch off
  { //turn off
    Serial.println("Turning off");  
    setStripOff();
    neopix.show();
    SW_ON = false;
  }
  else if(SW_ON && digitalRead(BUTTON) && !B_PRESS) //press button
  { //change colour
    Serial.println("Change next colour");  
    solidIndex++;
    setStrip(solidIndex);
    neopix.show();
    B_PRESS = true;
  }
  else if(!digitalRead(BUTTON) && B_PRESS) //let go button
  {
    Serial.println("Release button. No action required.");  
    B_PRESS = false;
  }
  yield();
  delay(1000);
}


void setStrip(int colour)
{
  if(colour >= SOLID_LIST_LEN)
  {
    solidIndex = 0;
    colour = 0;
  }
  for(int i=0; i<NUM_PIXELS; i++)
  {
    setColor(i,solidColours[colour][0], solidColours[colour][1], solidColours[colour][2]);
  }
}

void setStripOff()
{
  for(int i=0; i<NUM_PIXELS; i++)
  {
    setColor(i,offColour[0], offColour[1], offColour[2]);
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

