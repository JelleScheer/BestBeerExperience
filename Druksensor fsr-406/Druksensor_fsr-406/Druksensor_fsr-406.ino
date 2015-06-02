// include library code:
#include <LiquidCrystal.h>

// Initializeer de lcd scherm met de juiste output porten
// 8-rs
// 9-e
// 10~13 - d4~d7

// Druksensor aangesloten op analoogpin 0

LiquidCrystal lcd(8,9,10,11,12,13);

int druksensorreading = 0;

void setup() {
// set up the LCD’s number of columns and rows:
lcd.begin(16,2);

}
void loop() {

druksensorreading = analogRead(0);
 
lcd.setCursor(0, 0);
lcd.print(druksensorreading);
lcd.print("    ");

lcd.setCursor(0, 1);

if(druksensorreading < 780 ){
    lcd.print("Glas leeg!      ");
}
else{
    lcd.print("Glas vol        ");
}
}
