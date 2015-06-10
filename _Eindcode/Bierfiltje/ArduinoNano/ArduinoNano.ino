#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include<stdlib.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;


void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);

  // Start up the temperature library
  sensors.begin();
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures

  String theMessage = String(sensors.getTempCByIndex(0)); 
  int messageSize = theMessage.length();

  for (int i = 0; i < messageSize; i++)
  {
    int charToSend[1];
    charToSend[0] = theMessage.charAt(i);
    radio.write(charToSend, 1);
    Serial.print(theMessage.charAt(i));
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
