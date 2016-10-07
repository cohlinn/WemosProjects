#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 21
#define NEOPIXEL_PIN D7
//Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);
Adafruit_NeoPixel neopix(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW | NEO_KHZ800);

//TO BE MODIFIED according to need
//Parameters for Solid display
#define SOLID_LIST_LEN 7
byte solidColours[SOLID_LIST_LEN][3] = 
  {
    {0x10,0x10,0x10},
    {0x00,0x10,0x10},
    {0x00,0x00,0x10},
    {0x00,0x10,0x00},
    {0x10,0x00,0x00},
    {0x10,0x10,0x00},
    {0x10,0x00,0x10}
  };
int solidIndex=0;

//TO BE MODIFIED according to need
//Parameters for Gradual display
byte gradColours[NUM_PIXELS][4];
int gHead = 0;

void setup() 
{
  Serial.begin(115200);
  neopix.begin();

  //SolidColour sections
  solidIndex=0;

  //GradualColour sections
  gHead=0;
  setupGradPixels(NUM_PIXELS, 0x00, 0x00, 0x00);

  //Setup MRT lights (4 white, the rest blue)
  setupGradPixels(NUM_PIXELS, 0x05, 0x05, 0x0a);
  for(int i=0; i<4; i++)
    setGradPixel(i, 0x05, 0x05, 0x05);
}

void loop() 
{

  /* Start SolidColour sections
  solidColorCycle(NUM_PIXELS, solidColours, SOLID_LIST_LEN);
  // End SolidColor Sections */

  /* Start GradualColour section
  byte r, g, b;
  r = random(256);
  g = random(256);
  b = random(256);
  setGradPixel(gHead, r, g, b);
  Serial.printf("%d: %d %d %d \n", gHead, r, g, b);
  */

  gradColorCycle();
  // End GradualColour section */
  neopix.show();
  delay(100);
}

//==== Gradual Colour section
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


//======== Common functions
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

