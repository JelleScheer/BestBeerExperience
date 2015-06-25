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

/* Radio pinnen Mega
 * CSN - D10
 * MI - D12
 * MO - D11
 * SCK - D13
 * CE - D9
 * VCC - 3.3V
 * IRQ - niet gebruikt
 */

//Initialiseer radio
int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

//Boolean om tussen temperatuur en druk te wisselen
boolean wisseldata = false;

void setup(void)
{
  //Begin serial op baudrate 9600
  Serial.begin(9600);
  
  //Start radio op transmitting pipe
  radio.begin();
  radio.openWritingPipe(pipe);

  // Start temperatuursensor
  sensors.begin();
  
  // Start rgbstrip
  pixels.begin();
}

void loop(void)
{
  //Wisselt constant tussen temperatuur en druk, en stuurt deze op naar het hoofdpaneel
  if(wisseldata){
    //Vraag temperatuur op
    sensors.requestTemperatures();
    //Stuur de temperatuur op met een "t" op het einde (als indicatie voor hoofdpaneel dat het om de temperatuur gaat)
    sendStringToHoofdpaneel(String(sensors.getTempCByIndex(0)) + "t");
  }
  else{
    //Stuur de druk op met een "d" op het einde (als indicatie voor hoofdpaneel dat het om de druk gaat)
    sendStringToHoofdpaneel(String(analogRead(DRUKSENSORPINPUT)) + "d"); 
  }
  
  //Gebruik de opgehaalde temperatuur om op de RGBStrip de juiste kleur te laten zien
  for(int i=0;i<NUMPIXELS;i++)
  {
    //Als de temperatuur onder de 10 graden is
    if(sensors.getTempCByIndex(0) < 10)
    {
      //Zet de RGBStrip op de kleur groen
      pixels.setPixelColor(i, pixels.Color(101,211,245));
      pixels.show();
      delay(delayval);
    }
    //Als de temperatuur tussen de 10 en 15 graden is
    else if(sensors.getTempCByIndex(0) > 10 && sensors.getTempCByIndex(0) < 15)
    {
      //Zet de RGBStrip op de kleur oranje
      pixels.setPixelColor(i, pixels.Color(229,144,32));
      pixels.show();
      delay(delayval);
    }
    else 
    //Als de temperatuur hoger is dan 15 graden
    {
      //Zet de RGBStrip op de kleur rood
      pixels.setPixelColor(i, pixels.Color(201,68,28));
      pixels.show();
      delay(delayval);
    }
  }
  
  wisseldata = !wisseldata;
}

void sendStringToHoofdpaneel(String message){
  
  //Haal de message op die verstuurd moet worden, en converteer deze
  for (int i = 0; i < message.length(); i++)
  {
    int charToSend[1];
    charToSend[0] = message.charAt(i);
    radio.write(charToSend, 1);
    Serial.print(message.charAt(i));
  }
  Serial.println("");
  
  //Verstuur het bericht en stuur de "terminate string"
  msg[0] = 2;
  radio.write(msg, 1);

  //Zet radio even uit om noise te voorkomen
  radio.powerDown();
  delay(1000);
  radio.powerUp();
}
