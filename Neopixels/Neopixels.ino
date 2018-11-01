#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 10
#define NEOPIXEL_PIN D7
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);
//Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGBW | NEO_KHZ800);

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
  strip.begin();

//  //SolidColour sections
//  solidIndex=0;
//
//  //GradualColour sections
//  gHead=0;
//  setupGradPixels(NUM_PIXELS, 0x00, 0x00, 0x00);
//
//  //Setup MRT lights (4 white, the rest blue)
//  setupGradPixels(NUM_PIXELS, 0x05, 0x05, 0x0a);
//  for(int i=0; i<4; i++)
//    setGradPixel(i, 0x05, 0x05, 0x05);
}

void loop() 
{

  rainbowCycle(20);

//  strip.show();
  delay(100);
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//==== Gradual Colour section
/*void gradColorCycle()
{
 int j = gHead;
  for(int i=0; i<NUM_PIXELS; i++, j++)
  {//i=neopixel position, j=colour to assign to that pixel from gradColours
    j = j % SOLID_LIST_LEN;
    setColor(i,gradColours[j][0], gradColours[j][1], gradColours[j][2]);
  }
  gHead = abs(gHead+1) % NUM_PIXELS;
}

*/
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


\
