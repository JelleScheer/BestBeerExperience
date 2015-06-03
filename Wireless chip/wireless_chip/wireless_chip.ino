// include library code:
#include <LiquidCrystal.h>

// Initializeer de lcd scherm met de juiste output porten
// 8-rs
// 9-e
// 10~13 - d4~d7
//LiquidCrystal lcd(8,9,10,11,12,13);
LiquidCrystal lcd(52,50,48,46,44,42);

void setup() {
// set up the LCD’s number of columns and rows:
lcd.begin(16,2);
// Print a message to the LCD.
lcd.print("Barry - secondes");
}
void loop() {
// set the cursor to column 0, line 1
// (note: line 1 is the second row, since counting begins with 0):
lcd.setCursor(0, 1);
// print the number of seconds since reset:
lcd.print(millis()/1000);
}
