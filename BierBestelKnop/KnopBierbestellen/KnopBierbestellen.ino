
boolean bierisonderweg = false;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(18, INPUT);	   // Pin 18 is input to which a switch is connected = INT0
  attachInterrupt(5, bestelBier, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println("Loop");
  
  if(bierisonderweg){
      Serial.println("bier is onderweg alcoholist");
      bierisonderweg = false;
  }
}

void bestelBier() {
  bierisonderweg = true;  
}
