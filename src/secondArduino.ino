#include <LiquidCrystal.h>

char mystr[12];

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
}


void loop() {
    lcd.setCursor(0, 1); 
	Serial.readBytes(mystr, 12);
 	lcd.print(mystr);
    Serial.println(mystr);
 	delay(1000);
}
 