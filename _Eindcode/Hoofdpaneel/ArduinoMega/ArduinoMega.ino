#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <LiquidCrystal.h>
 
/*
This sketch receives strings from sending unit via nrf24 
and prints them out via serial.  The sketch waits until
it receives a specific value (2 in this case), then it 
prints the complete message and clears the message buffer.
*/

int msg[1];
RF24 radio(53,48);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;
String theMessage = "";

LiquidCrystal lcd(8,9,10,11,12,13);

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
}

void loop(void)
{
  if (radio.available())
  {
    bool done = false;  
    done = radio.read(msg, 1); 
    char theChar = msg[0];
    
    if (msg[0] != 2)
    {
      theMessage.concat(theChar);
    }
    else 
    {
        Serial.println(theMessage);
        lcd.clear();
        if(theMessage.endsWith("t"))
        {
          
          theMessage.remove(theMessage.length() -1);
          lcd.print("Celcius: " + theMessage);
        }
        else if(theMessage.endsWith("d"))
        {
          theMessage.remove(theMessage.length() -1);
          lcd.print("Druk: " + theMessage); 
        }
        else
        {
          lcd.print("Onbekende data, ruis?");
        }
      
      theMessage= "";
    }
  }
}
