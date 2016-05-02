void printDefaultLCDText() {
  //prints the default lcd text
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(10, 0);
  lcd.write(223);
  lcd.setCursor(11, 0);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Agua:");
  lcd.setCursor(10, 1);
  lcd.write(223);
  lcd.setCursor(11, 1);
  lcd.print("C");
}
