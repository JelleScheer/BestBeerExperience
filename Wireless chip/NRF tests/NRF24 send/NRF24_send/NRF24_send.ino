#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
 
/*
This sketch sends a string to a corresponding Arduino
with nrf24 attached.  It appends a specific value 
(2 in this case) to the end to signify the end of the
message.
*/
 
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
<<<<<<< HEAD
  String theMessage = "Hello thare!"; 
void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);}
void loop(void){
  int messageSize = theMessage.length();
  for (int i = 0; i < messageSize; i++) {
=======
String theMessage = "Hello thare!"; 

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
}

void loop(void)
{
  int messageSize = theMessage.length();
  for (int i = 0; i < messageSize; i++) 
  {
>>>>>>> origin/BranchBarry
    int charToSend[1];
    charToSend[0] = theMessage.charAt(i);
    radio.write(charToSend,1);
  }  
<<<<<<< HEAD
//send the 'terminate string' value...  
  msg[0] = 2; 
  radio.write(msg,1);
=======
  
//send the 'terminate string' value...  
msg[0] = 2; 
radio.write(msg,1);
  
>>>>>>> origin/BranchBarry
/*delay sending for a short period of time.  radio.powerDown()/radio.powerupp
//with a delay in between have worked well for this purpose(just using delay seems to
//interrupt the transmission start). However, this method could still be improved
as I still get the first character 'cut-off' sometimes. I have a 'checksum' function
on the receiver to verify the message was successfully sent.
*/
<<<<<<< HEAD
  radio.powerDown(); 
  delay(1000);
  radio.powerUp();
=======
radio.powerDown(); 
delay(1000);
radio.powerUp();
>>>>>>> origin/BranchBarry
}
