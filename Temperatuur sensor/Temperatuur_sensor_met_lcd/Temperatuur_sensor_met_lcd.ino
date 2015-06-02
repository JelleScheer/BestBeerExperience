#include <OneWire.h>
#include <DallasTemperature.h>

// include library code:
#include <LiquidCrystal.h>
 
 
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
 
 // Initializeer de lcd scherm met de juiste output porten
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
}
 
 
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures


  lcd.setCursor(0, 1);
  lcd.print(sensors.getTempCByIndex(0)); 
  lcd.print(" C");
 
}
