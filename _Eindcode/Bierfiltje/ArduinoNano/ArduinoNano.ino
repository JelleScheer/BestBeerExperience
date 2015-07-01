#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<stdlib.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

// include neopixel library:
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Define druksensor analoog pin
#define DRUKSENSORPINPUT 0

// Define NeoPixel requirements
#define PIXEL_PIN      6
#define PIXEL_COUNT    30

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Initialise RGBStrip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

int checkColor = 0;

boolean wisseldata = false;

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);

  // Start up the temperature library
  sensors.begin();
  
  // Start up RGBStrip
  strip.begin();
  strip.show();
  rainbow(20);
}

void loop(void)
{
  if(wisseldata){
    sensors.requestTemperatures(); // Send the command to get temperatures
    sendStringToHoofdpaneel(String(sensors.getTempCByIndex(0)) + "t");
  }
  else{
    sendStringToHoofdpaneel(String(analogRead(DRUKSENSORPINPUT)) + "d"); 
  }
  
  if(checkColor == 5)
  {
    //SLAAPMODUS
    //if(analogRead(DRUKSENSORPINPUT) > 500)
    //  {
    //    colorWipe(strip.Color(0, 0, 255), 50); //Blue
    //    delay(1000);
    //    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    //    sleep_enable();
    //    sleep_mode();
    //  }
    if(sensors.getTempCByIndex(0) < 23)
      {
        colorWipe(strip.Color(0, 0, 255), 50); //Blue
      }
      else if(sensors.getTempCByIndex(0) > 23 && sensors.getTempCByIndex(0) < 28)
      {
        colorWipe(strip.Color(227, 145, 14), 50); // Orange
      }
      else 
      {
        colorWipe (strip.Color(194,13,10), 50); // Red
      }
      
      checkColor = 0;
  }
    
    checkColor = checkColor + 1;
    
    wisseldata = !wisseldata;
}

void colorWipe(uint32_t c, uint8_t) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void sendStringToHoofdpaneel(String message){

  for (int i = 0; i < message.length(); i++)
  {
    int charToSend[1];
    charToSend[0] = message.charAt(i);
    radio.write(charToSend, 1);
    Serial.print(message.charAt(i));
  }
  Serial.println("");
  
  //send the 'terminate string' value...
  msg[0] = 2;
  radio.write(msg, 1);

  /*delay sending for a short period of time.  radio.powerDown()/radio.powerupp
  //with a delay in between have worked well for this purpose(just using delay seems to
  //interrupt the transmission start). However, this method could still be improved
  as I still get the first character 'cut-off' sometimes. I have a 'checksum' function
  on the receiver to verify the message was successfully sent.
  */
  radio.powerDown();
  delay(1000);
  radio.powerUp();
}
