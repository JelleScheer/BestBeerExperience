#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include "Timer.h"

//Initialiseer radio
int msg[1];
RF24 radio(53, 48);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int lastmsg = 1;
String theMessage = "";

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

//Bool bier bestelling bijhouden
boolean verstuurBierBestelling = false;

//Variabele die bijhoud hoeveel bier iedereen opheeft
int aantalBierOp = 0;

//Boolean voor bijhouden of er een bestelling is geplaatst
boolean isBestellingGeplaatst = false;

//Variabele die de sensor data bijhouden
String temperatuurSensorRead = "";
String drukSensorRead = "";

//Tijdelijke variabele die de string omzetten in int
int temperatuurSensorReadInt = 0;
int drukSensorReadInt = 0;

//Variabele die bijhoud wel scherm getoont moet worden
int huidigScherm = 0;

//Timer om lcd scherm te updaten
Timer t;

//Debounce variabelen
long debouncing_time = 100; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

void setup(void)
{
  //Begin serial op baud rate 9600
  Serial.begin(9600);

  //Begin radio als receiving
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  //Begin LCD scherm
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Best Beer");
  lcd.setCursor(0, 1);
  lcd.print("Experience");
  lcd.setCursor(0, 0);

  // Init knoppen als input
  pinMode(2, INPUT_PULLUP);	   // Pin 2 aanknop
  pinMode(3, INPUT_PULLUP);	   // Pin 3 uitknop
  pinMode(21, INPUT_PULLUP);      //Pin 21 bierbestelknop

  //Interrupts toevoegen aan de knoppen
  attachInterrupt(0, systeemAan, FALLING );
  attachInterrupt(1, systeemUit, FALLING );
  attachInterrupt(2, debounceBestelInterrupt, FALLING );

  //Activeer timer
  t.every(5000, updateLCDScherm);
}

void loop(void)
{
  //Check of systeem aan staat
  if (systeemIsAan) {
    //Update timer
    t.update();

    //Als de uit-message op het LCD scherm stond, clear deze
    if (lcdUitMessageDisplayed) {
      //Zet reading pipe open, mocht hij net uit de uitstand komen
      radio.openReadingPipe(1, pipe);
      radio.startListening();

      lcdUitMessageDisplayed = false;
      lcd.clear();
      lcd.print("Connectie maken");
      lcd.setCursor(0, 1);
      lcd.print("Even geduld...");
    }

    //Als er een radio verbinding aanwezig is
    if (radio.available())
    {
      //Open nogmaals de pipe als receiver (mocht hij in transmitting status staan)
      radio.openReadingPipe(1, pipe);
      radio.startListening();

      //Haal de message op, zet deze in een string
      bool done = false;
      done = radio.read(msg, 1);
      char theChar = msg[0];

      //Initialisatie message
      if (msg[0] != 2)
      {
        theMessage.concat(theChar);
      }
      //Geen initialisatie message
      else
      {
        //Print de message in console
        Serial.println(theMessage);

        //Als het bericht eindigt met een t
        if (theMessage.endsWith("t"))
        {
          //Haal de T weg, print op het scherm Celcius: temperatuur
          theMessage.remove(theMessage.length() - 4);

          temperatuurSensorRead = theMessage;
        }
        //Als het bericht eindigt met een d
        else if (theMessage.endsWith("d"))
        {
          //Haal de D weg, print op het scherm Druk: druk
          theMessage.remove(theMessage.length() - 1);

          drukSensorRead = theMessage;
        }
        //Als het een ander bericht is, print in console "ruis" voor debugging
        else
        {
          Serial.println("Ruis!");
        }

        theMessage = "";
      }
    }
   
    //Als er op de bierbestelknop is gedrukt
    if (verstuurBierBestelling == true)
    {   
      isBestellingGeplaatst = !isBestellingGeplaatst;
      Serial.println(isBestellingGeplaatst);
      
      //Switch de pipe naar transmitting
      radio.openWritingPipe(pipe);

      Serial.println("Swtiching to transmitting. . .");

      //Reset bier bestel boolean
      verstuurBierBestelling = false;

      //Stuur bericht naar KeukenArduino dat bestelLED aan moet gaan
      sendStringToHoofdpaneel(String("lon"));

      //Switch de pipe weer terug naar receiving
      radio.openReadingPipe(1, pipe);
      radio.startListening();
      
      drukSensorReadInt = drukSensorRead.toInt();
      
      //Verhoog het aantal gedronken bier per filtje
      if(!isBestellingGeplaatst)
      {
        if( drukSensorReadInt >= 100 && drukSensorReadInt <= 250)
        {
          aantalBierOp++;
        }
      }
    }
  }
  //Als er op de UIT knop wordt gedrukt
  else {
    //Laat standaard bericht op LCD scherm zien
    if (!lcdUitMessageDisplayed) {
      lcd.clear();
      lcd.print("Turn on for Best");
      lcd.setCursor(0, 1);
      lcd.print("Beer Experience");

      lcdUitMessageDisplayed = true;

      //Open transmitting pipe en zend bericht naar KeukenArduino om het bestelLED uit te zetten (mocht hij nog aan staan)
      radio.openWritingPipe(pipe);
      sendStringToHoofdpaneel("suit");
    }

    delay(1000);
  }


}

void sendStringToHoofdpaneel(String message) {

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

//Interrupt van de aanknop
void systeemAan() {
  systeemIsAan = true;
}

//Interrupt van de uitknop
void systeemUit() {
  systeemIsAan = false;
  
  //Reset variabelen
  huidigScherm = 0;
  aantalBierOp = 0;
  isBestellingGeplaatst = false;
  temperatuurSensorRead = "";
  drukSensorRead = "";
}


void debounceBestelInterrupt() {
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    bierBestel();
    last_micros = micros();
  }
}

//Interrupt van de bierbestelknop
void bierBestel() {
  verstuurBierBestelling = true;
}

void updateLCDScherm() {
  if (temperatuurSensorRead != "" && drukSensorRead != "")
  {

    lcd.clear();

    switch (huidigScherm) {
      case 0:
        lcd.print("Bierfiltje 1 is");
        lcd.setCursor(0, 1);
        lcd.print(temperatuurSensorRead + " graden C");
        break;

      case 1:
        lcd.print("Bierfiltje 1 is");
        lcd.setCursor(0, 1);

        temperatuurSensorReadInt = temperatuurSensorRead.toInt();

        if (temperatuurSensorReadInt <= 0) 
        {
          lcd.print("bevroren");
        } 
        else if (temperatuurSensorReadInt >= 1 && temperatuurSensorReadInt <= 10) 
        {
          lcd.print("perfecte temp.");
        } 
        else if (temperatuurSensorReadInt > 10 && temperatuurSensorReadInt <= 15) 
        {
          lcd.print("lauw");
        } 
        else 
        {
          lcd.print("te warm");
        }
        break;

      case 2:
        lcd.print("Bierfiltje 1");
        lcd.setCursor(0, 1);

        temperatuurSensorReadInt = temperatuurSensorRead.toInt();

        if (temperatuurSensorReadInt <= 0) 
        {
          lcd.print("bier is te koud");
        } 
        else if (temperatuurSensorReadInt > 0 && temperatuurSensorReadInt <= 10) 
        {
          lcd.print("ultieme genot");
        } 
        else if (temperatuurSensorReadInt > 10 && temperatuurSensorReadInt <= 15) 
        {
          lcd.print("drink sneller");
        } 
        else 
        {
          lcd.print("bier is te warm");
        }
        break;

      case 3:
        lcd.print("Persoon 1 heeft");
        lcd.setCursor(0, 1);
        lcd.print(String(aantalBierOp) + " bier op");

        break;

      case 4:
        if (aantalBierOp < 2) 
        {
          lcd.print("Persoon 1 is");
          lcd.setCursor(0, 1);
          lcd.print("nuchter");
        }
        else if (aantalBierOp >= 2 && aantalBierOp <= 7) 
        {
          lcd.print("Persoon 1 is");
          lcd.setCursor(0, 1);
          lcd.print("aangeschoten");
        }
        else if (aantalBierOp > 7 && aantalBierOp <= 15 ) 
        {
          lcd.print("Persoon 1 is");
          lcd.setCursor(0, 1);
          lcd.print("dronken");
        }
        else if (aantalBierOp > 15 ) 
        {
          lcd.print("Persoon 1 is in");
          lcd.setCursor(0, 1);
          lcd.print("levensgevaar");
        }

        break;
    }
  }

  if (huidigScherm < 4 )
  {
    huidigScherm++;
  }
  else
  {
    huidigScherm = 0;
  }

}
