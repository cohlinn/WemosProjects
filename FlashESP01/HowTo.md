# How to flash ESP-01 without FTDI cable
## Using Wemos D1 Mini
Assumption: The reader already knows how to use Arduino IDE, has some basic electronics, and knows how to use breadboard and dupont jumper wires. 

# A. Setup the IDE
1.	Install Arduino IDE
2.	Add ESP8266 Board Manager
3.	Detailed instructions on how to install Arduino IDE and how to include ESP8266 Board Manager, please refer to https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/


# B. Wire up ESP-01 to Wemos
1. How to connect the ESP8266 and Wemos D1 Mini.

2. This is the ESP-01 pinout for your reference:

![ESP8266 Pinout](https://cordobo.com/wp-content/uploads/esp8266_pinout.png)
 
3.	The idea is to wire up a circuit to convert the Wemos D1 mini into an USB-to-Serial converter, which we then use to flash the ESP8266 with the new program from Arduino IDE.

4.	To do this, we make use of the Wemos' :
  -	RX and TX pins for communications
  -	3.3V and GND pins for power. Do not use the 5V pin. 

5. ESP8266 is a 3.3V board. And the Wemos conveniently converted the 5V USB power to 3.3V for us.

6.	Connect the Wemos to the ESP-01 using the wiring scheme below.

Wiring scheme:

Wemos D1 mini	| ESP8266 (ESP-01)
--------------|-----------------
RX |	RX
TX | TX 
GND |	GND 
GND | GPIO 0
3.3V |	VCC
3.3V |	CH_PD
Reset	| Reset

You might need a breadboard to allow multiple connections to GND and 3.3V pins.

> Note 1: Slight difference from this tutorial to the Arduino Uno one, is that I also wired the Wemos's reset (RST) pin to the ESP-01 reset pin. It makes life easier when I do multiple updates during prototyping, because the Wemos board will automatically send the Reset signal prior to flashing the ESP-01. Before this, I used a jumper wire to act as my Reset switch to put the ESP-01 into "programming" mode, or a home-made board with reset button.

> Note 2: The ESP-01 will not sit properly on the breadboard as shown here. The distance between the two rows are not wide enough. To overcome this, I built my own ESP-01 jig with a variable board, an 8-pin male and 8-pin female headers. I broke the 8-pin strip into 4-pin strips. This is what it looks like.

7.	Connect the Wemos to the computer with USB cable.

# C. Setup Arduino IDE to flash ESP-01
1.	Select Board as "Generic ESP8266 Module".
 
2.	Select the flash size. If in doubt, pick the 512K (64K SPIFFS).
 
> Note that there are different versions of ESP-01.

> In general, the blue pcbs has 512KB memory (3a), and the black pcbs has 1 MB memory(3b). If in doubt, use the lower option 512KB . The black pcbs version can be flashed with the lower setting, you just would not get its full potential.

3.	Set the upload speed to 115200.
 
4. Pick the COM port (USB port). 

>	If you have multiple COM ports listed like me, and you are not sure which one to use, unplug all your other USB devices, so that only the Wemos D1 Mini is plugged in via USB.

5.	Load the code and flash
 
# D. Done.
The example code is a modified Arduino sample Blink code. I had simply changed the output pin to GPIO-2 of the ESP-01 board.

If you setup your LED as I did, (GPIO-2 = LED positive, GND = LED negative), the LED will start blinking.

# Credits and References
This idea started when a few ESP-01 came into my possession. At that time, I had several Wemos boards and had been working with them for a few months. 

What I did not have was FTDI cables and Arduino Uno. Most tutorials to flash an ESP-01 involved an FTDI cable. There was one that uses Arduino Uno. And this project was based on that tutorial.

Since I was using Wemos as a miniaturised Uno with WIFI capabilities, I thought the concept behind the tutorial (use the built-in FTDI in the Wemos) would work.

It was actually simpler than using an Uno. And one difference is that I used the Wemos' RESET pin to also control the ESP-01 RESET pin.
https://cordobo.com/2300-flash-esp8266-01-with-arduino-uno/

The ESP-01's pin configurations make it difficult to use on a breadboard. I built my own adapter using male and female headers and a variable board. But you don't really need it.

You can just use female headers to connect from breadboard to the ESP-01. The wires get rather messy, but it works.
