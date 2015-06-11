boolean uitdoen = false;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(2, INPUT);	   // Pin 2 is input to which a switch is connected = INT0
  pinMode(3, INPUT);	   // Pin 3 is input to which a switch is connected = INT0
  attachInterrupt(0, systeemAan, RISING);
  attachInterrupt(1, systeemUit, RISING);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (uitdoen) { 
    uidoen = false;
    // De code van jelle moet hier nog komen om het systeem uit te zetten
  }
  
  
  delay(1000);
  Serial.println("Loop");
  
  if(knopUit){
      Serial.println("uit");
      bierisonderweg = false;
  }
}

void systeemAan() {
  // Deze code word pas na jelles toevoeging aangevuld
}

void systeemUit() {
  uidoen = true;
} 
