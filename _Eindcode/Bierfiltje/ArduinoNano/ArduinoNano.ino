#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<stdlib.h>

// include neopixel library:
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Define druksensor analoog pin
#define DRUKSENSORPINPUT 0

// Define NeoPixel requirements
#define PIN            6
#define NUMPIXELS      30

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Initialise RGBStrip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); 
int delayval = 1; // delay for half a second

int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

boolean wisseldata = false;

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);

  // Start up the temperature library
  sensors.begin();
  
  // Start up RGBStrip
  pixels.begin();
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
  
  // use gathered temperature to select the right colour
  for(int i=0;i<NUMPIXELS;i++)
  {
    if(sensors.getTempCByIndex(0) < 10)
    {
      pixels.setPixelColor(i, pixels.Color(101,211,245)); // Green
      pixels.show();
      delay(delayval);
    }
    else if(sensors.getTempCByIndex(0) > 10 && sensors.getTempCByIndex(0) < 15)
    {
      pixels.setPixelColor(i, pixels.Color(229,144,32)); // Orange
      pixels.show();
      delay(delayval);
    }
    else 
    {
      pixels.setPixelColor(i, pixels.Color(201,68,28)); // Red
      pixels.show();
      delay(delayval);
    }
  }
  
  wisseldata = !wisseldata;
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
