#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

/*
This sketch receives strings from sending unit via nrf24
and prints them out via serial.  The sketch waits until
it receives a specific value (2 in this case), then it
prints the complete message and clears the message buffer.
*/

int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;
String theMessage = "";

/* Radio pinnen Mega
 * CSN - D10
 * MI - D12
 * MO - D11
 * SCK - D13
 * CE - D9
 * VCC - 3.3V
 * IRQ - niet gebruikt
 */

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
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
      
      if (theMessage.endsWith("lon"))
      {
        theMessage.remove(theMessage.length() - 3);
        
        //Led aan
      }
      else if (theMessage.endsWith("loff"))
      {
        theMessage.remove(theMessage.length() - 4);
        
        //Led uit
      }

      theMessage = "";
    }
  }
}
