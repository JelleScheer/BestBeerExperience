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

int ledNET = 3;
int ledBESTEL = 4;
boolean ledon = 0;

void setup(void)
{
  pinMode(ledNET, OUTPUT);
  pinMode(ledBESTEL, OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop(void)
{
  if (radio.available())
  {
    //Network status led on
    digitalWrite(3, HIGH);
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
      
      if (theMessage.endsWith("on"))
      {
        if (ledon == 0)
        {
          digitalWrite(4, HIGH);
          ledon = 1;
        }
        else if (ledon == 1)
        {
          digitalWrite(4, LOW);
          ledon = 0; 
        }
      }

      theMessage = "";
    }
  }
}
