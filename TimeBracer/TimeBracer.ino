/*
 * Meant for LTA Appreciation Night 2018.
 * Flashes nice friendly colours normally. 
 * When people get too close, will flash angry red.
 */
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char* ssid = "wemos";
const char* password = "secret";

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);

#define NUM_PIXELS 8
#define NEOPIXEL_PIN D4
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_RGB | NEO_KHZ800);

//Param for happy
#define HAPPY_LEN 8
byte happy[HAPPY_LEN][3] = 
  {
    {0x60,0x40,0x00},
    {0x00,0x90,0x00},
    {0x00,0x80,0x50},
    {0x00,0x00,0x90},
    {0x9F,0x00,0x90},
    {0xA4,0x00,0x00},
    {0x9B,0x20,0x00},
    {0x9B,0x20,0x00}
  };
int colorIndex=0;

byte angry[3] = {250, 0, 0};
byte warning[3] = {250, 100, 0};

void setup() {
  Serial.begin(115200);
  strip.begin(); // This initializes the NeoPixel library.
  strip.show();
  Serial.println("Initialising ....");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

}

int show=9; //default pattern
int cycle=0;

void loop() {
  yield();
  server.handleClient();
//   Serial.print("Starting colour =");
//  Serial.println(colorIndex);
//  gradColorCycle(colorIndex);
//  //gradColorCycle(0);
//  pixels.show();
//  colorIndex = abs(colorIndex+1) % NUM_PIXELS;
//  delay(1000);

  //startShow(value++);
  //value=value%10;
  if(show!=11)
    startShow(show);
  else
  {
    startShow(cycle+1);
    cycle =(cycle+1) % 10;
  }
  //delay(1000);
}

void handleRoot() {
  server.send(200, "text/html", "<style> \
  .button {  \
    border: none;   \
    color: black;  \
    padding: 10px 10px;  \
    text-align: center;  \
    text-decoration: none;  \
    display: inline-block;  \
    font-size: 20px; } \
  </style> \
  <H1> <FONT size=20> \
  <form action='http://192.168.4.1'> \
  <button name='show' value=0>OFF</button> \
  <BR/><button name='show' value=1>RED</button> \
  <button name='show' value=2>GREEN</button> \
  <button name='show' value=3>BLUE</button> \
  <BR/>CHASE <BR/> \
  <button name='show' value=4>WHITE</button> \
  <button name='show' value=5>RED</button> \
  <button name='show' value=6>GREEN</button> \
  <button name='show' value=7>BLUE</button> \
  <BR/><button name='show' value=8>RAINBOW</button> \
  <BR/><button name='show' value=9>RAINBOW CYCLE</button> \
  <BR/><button name='show' value=10>CHASE RAINBOW</button> \
  <BR/><button name='show' value=11>ALL</button> \
  </form> \
  </font></H1>");
  String input = server.arg("show");
  show = input.toInt();
  if(show==11)
  {
    cycle = 0;
  }
  else
    startShow(show);
}

void startShow(int i) {
  switch(i){
    case 0: //colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
              colorWipe(strip.Color(0, 20, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(60, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 30, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 100), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(30, 30, 30), 50); // White
            break;
    case 5: theaterChase(strip.Color(60,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   60, 0), 50); // Green
            break;
    case 7: theaterChase(strip.Color(  0,   0, 100), 50); // Blue
            break;
    case 8: rainbow(20);
            break;
    case 9: rainbowCycle(20);
            break;
    case 10: theaterChaseRainbow(100);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


void rainbow(uint8_t wait) {
uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      //strip.setPixelColor(i, Wheel((i+j) & 255));
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    yield();
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    yield();
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    yield();
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  int brightness = 50; //default 5; lower=brighter
  if(WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3)/brightness, 0, (WheelPos * 3)/brightness);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3)/brightness, (255 - WheelPos * 3)/brightness);
  }
  WheelPos -= 170;
  return strip.Color((WheelPos * 3)/brightness, (255 - WheelPos * 3)/brightness, 0);
}


//===============================
/*

void pulse(byte mood[])
{
  int middle=(NUM_PIXELS/2);
  int midlow=-1;
  Serial.print("Middle =");
  Serial.println(middle);
  if((NUM_PIXELS%2) == 0) //even number of leds
  { //two middles = middle & middle-1
    midlow=middle-1;
    setColor(midlow,mood[0], mood[1], mood[2]);
    setColor(middle,mood[0], mood[1], mood[2]);
  }
  else
  {
    setColor(middle,mood[0], mood[1], mood[2]);
  }

    byte r,g,b;
    r=mood[0];
    g=mood[1];
    b=mood[2];
  //Odd number of LEDs
  if((NUM_PIXELS%2) == 1) //Odd number of leds
  {
    for(int i=1; i<=middle; i++)
    {
      r=(r-50)>0? r-50: 0;
      g=(g-50)>0? g-50: 0;
      b=(b-50)>0? b-50: 0;
      setColor(middle-i,r, g, b);
      setColor(middle+i,r, g, b);
    }
  }
  
  //even number of LEDs
  else
  {
    for(int i=1; i<middle; i++)
    {
      setColor(midlow-i,mood[0], mood[1], mood[2]);
      setColor(middle+i,mood[0], mood[1], mood[2]);
    }
  }
  
  pixels.show();
  delay(1000);
}



//==== Gradual Colour section
void gradColorCycle(int colorIndex)
{
 int j = colorIndex;
  for(int i=0; i<NUM_PIXELS; i++, j++)
  {//i=neopixel position, j=colour to assign to that pixel from gradColours
    j = j % HAPPY_LEN;
    setColor(i,happy[j][0], happy[j][1], happy[j][2]);
  }
}

void setColor(int pos, byte R, byte G, byte B)
{
  pixels.setPixelColor(pos, pixels.Color(R,G,B));
}

*/
