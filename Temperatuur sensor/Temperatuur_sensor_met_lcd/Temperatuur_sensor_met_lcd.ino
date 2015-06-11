#include <OneWire.h>
#include <DallasTemperature.h>

// include neopixel library:
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

// include library code:
#include <LiquidCrystal.h>
 
// Define NeoPixel requirements
#define PIN            6
#define NUMPIXELS      30
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Initialise RGBStrip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); 
int delayval = 1; // delay for half a second
 
 // Initialise LCD Screen on the right ports
// 8-rs
// 9-e
// 10~13 - d4~d7
LiquidCrystal lcd(8,9,10,11,12,13);
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
 
void setup(void)
{
  // set up the LCD’s number of columns and rows:
  lcd.begin(16,2);
  
  lcd.print("Temperatuur in C");

  // Start up the library
  sensors.begin();
  
  // Start up RGBStrip
  pixels.begin();
}
 
 
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures

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

  lcd.setCursor(0, 1);
  lcd.print(sensors.getTempCByIndex(0)); 
  lcd.print(" C");
}
