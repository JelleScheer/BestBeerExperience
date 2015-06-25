#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

//Initialiseer de radio chip
int msg[1];
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;
String theMessage = "";

/* Radio pinnen NANO Keuken
 * CSN - D10
 * MI - D12
 * MO - D11
 * SCK - D13
 * CE - D9
 * VCC - 3.3V
 * IRQ - niet gebruikt
 */

//led om netwerk verbinding aan te geven
int ledNET = 3;

//led om bier bestelling aan te geven
int ledBESTEL = 4;
boolean ledon = 0;

void setup(void)
{
  //Maak poort 3 en 4 output en zet de LEDs op laag
  pinMode(ledNET, OUTPUT);
  pinMode(ledBESTEL, OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  
  //Begin radio signaal op baudrate 9600
  Serial.begin(9600);
  radio.begin();
  
  //Zet de radio op receiving
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop(void)
{
  //Als er een radio verbinding beschikbaar is
  if (radio.available())
  {
    //Zet ledNET op hoog om verbinding aan te geven
    digitalWrite(3, HIGH);
    bool done = false;
    //Lees de opgehaalde message
    done = radio.read(msg, 1);
    char theChar = msg[0];

    //Initialisatie message
    if (msg[0] != 2)
    {
      theMessage.concat(theChar);
    }
    else
    {
      //Print de message in console
      Serial.println(theMessage);
      
      //Als message "on" is
      if (theMessage.endsWith("on"))
      {
        //En de bestelLED nog niet aan stond
        if (ledon == 0)
        {
          //Zet bestelLED aan
          digitalWrite(4, HIGH);
          delay(2000); //Debounce
          ledon = 1;
        }
        //Anders zet bestelLED weer uit (bestelling afgerond)
        else if (ledon == 1)
        {
          digitalWrite(4, LOW);
          ledon = 0; 
        }
      }
      //Als message "uit" is, gaat hoofdpaneel uit en dus moet het bestelLED ook uit gaan
      else if(theMessage.endsWith("uit"))
      {
        if (ledon == 1)
        {
          digitalWrite(4, LOW);
          ledon = 0;
        }
      }
      
      //Standaard is theMessage (string) leeg
      theMessage = "";
    }
  }
}
