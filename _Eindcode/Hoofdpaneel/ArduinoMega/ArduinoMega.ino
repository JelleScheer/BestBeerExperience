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
RF24 radio(53, 48);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;
String theMessage = "";
boolean verstuurBierBestelling = false;

// Initializeer de lcd scherm met de juiste output porten
// 31-rs
// 30-e
// 32~35 - d4~d7
LiquidCrystal lcd(31, 30, 32, 33, 34, 35);

/* Radio pinnen Mega
 * CSM - 48
 * MI - 50
 * MO - 51
 * SCK - 52
 * CE - 53
 * VCC - 3.3V
 * IRQ - niet gebruikt
 */

//Bool systeem status
boolean systeemIsAan = true;

boolean lcdUitMessageDisplayed = false;



void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  // Init knoppen als input
  pinMode(2, INPUT_PULLUP);	   // Pin 2 is input to which a switch is connected = INT0
  pinMode(3, INPUT_PULLUP);	   // Pin 3 is input to which a switch is connected = INT0
  pinMode(21, INPUT_PULLUP);      //Pin 21 is input to which a switch is connected = INT2

  //Interrupts toevoegen aan de knoppen
  attachInterrupt(0, systeemAan, FALLING );
  attachInterrupt(1, systeemUit, FALLING );
  attachInterrupt(2, bierBestel, FALLING );
}

void loop(void)
{
  if (systeemIsAan) {
    if (lcdUitMessageDisplayed) {
      lcdUitMessageDisplayed = false;
      lcd.clear();
    }

    if (radio.available())
    {
      radio.openReadingPipe(1, pipe);
      radio.startListening();

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
        if (theMessage.endsWith("t"))
        {

          theMessage.remove(theMessage.length() - 1);
          lcd.print("Celcius: " + theMessage);
        }
        else if (theMessage.endsWith("d"))
        {
          theMessage.remove(theMessage.length() - 1);
          lcd.print("Druk: " + theMessage);
        }
        else
        {
          lcd.print("Onbekende data, ruis?");
        }

        theMessage = "";
      }
    }
    if (verstuurBierBestelling == true)
    {
      radio.openWritingPipe(pipe);

      Serial.println("Writing!");

      verstuurBierBestelling = false;

      sendStringToHoofdpaneel(String("lon"));

      radio.openReadingPipe(1, pipe);
      radio.startListening();
    }
  }
  else {
    if (!lcdUitMessageDisplayed) {
      lcd.clear();
      lcd.print("Turn on for");
      lcd.setCursor(0, 2);
      lcd.print("Best Beer Experience");

      lcdUitMessageDisplayed = true;
      
      radio.openWritingPipe(pipe);
      sendStringToHoofdpaneel("suit");
      radio.openReadingPipe(1, pipe);
      radio.startListening();
    }

    delay(1000);
  }


}

void sendStringToHoofdpaneel(String message) {

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

void systeemAan() {
  systeemIsAan = true;

  Serial.println("AAN");
}

void systeemUit() {
  systeemIsAan = false;
}

void bierBestel() {
  verstuurBierBestelling = true;
}
