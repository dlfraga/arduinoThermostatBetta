#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
//LM35 port
const int LM_35_APIN = 5;
// DS18B20 port / one wire bus
const int ONE_WIRE_BUS = 7;
//relay pin on the board
const int RELAY_PIN = 6;
//update interval for main logic
const long UPDATE_INTERVAL = 2000;
//used to mantain the state machine
long previousMillis = 0;
//target temperature.
const float targetTemp = 26;
//temperature tolerance. The program will try to maintain the temperature beetween the targetTemp - tolerance $$
const float TEMP_TOLERANCE = 1;
// Onewire bus
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//ambient temperature
float tempAmbient = 0;
//water temperature
float tempWater = 0;
//is the target temperature achieved
bool tempAchieved = true;
//controls the relay state
int rlstate = LOW;
void loop()
{
  //To avoid using delay() we use a state machine
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > UPDATE_INTERVAL) {
    previousMillis = currentMillis;    
    sensors.requestTemperatures();
    tempAmbient = sensors.getTempC(sensor1);
    tempWater = read_lm35_temperature(LM_35_APIN);
    
   //delay reactivating the relay until we are completely below the target temp
   //this is done to avoid excessive flapping of the relay
    if (tempAchieved) {
      if (tempWater < (targetTemp - TEMP_TOLERANCE)) {
        tempAchieved = false;
      }
    } else {
      if (tempWater < (targetTemp - TEMP_TOLERANCE) || tempWater < (targetTemp + TEMP_TOLERANCE) ) {
        rlstate = HIGH;                
      } else {
        rlstate = LOW;
        tempAchieved = true;
      }
    }
    digitalWrite(RELAY_PIN, rlstate);
  }
  //update the lcd with the current temperatures
  lcd.setCursor(5, 0);
  lcd.print(tempAmbient);
  lcd.setCursor(5, 1);
  lcd.print(tempWater);  
}

void setup()
{
  //setup relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  setupSensors();
  printDefaultLCDText();
}

float read_lm35_temperature(int lmanalogpin)
{
  double referenceVoltage = 1.1; //Set to 5, 3.3, 2.56 or 1.1 depending on analogReference Setting
  float tempInC = 0;
  int reading = 0;
  for (int i = 0; i < 10; i++) { // Average 10 readings to increase accuracy
    reading += analogRead(lmanalogpin);
  }
  tempInC =  (referenceVoltage * reading * 10) / 1023;
  return tempInC;
}

void setupSensors() {
  // Set Analog reference to 1.1V this gives more accuracy to read the LM35 since the sensor will output 0-1 V
  // This only available on ATmega168 or ATmega328)
  // For more information see: http://arduino.cc/en/Reference/AnalogReference
  analogReference(INTERNAL);
  //initializes the onewire sensors
  sensors.begin();
  //validate if the ds sensor is connected
  if (!sensors.getAddress(sensor1, 0))
  {
    lcd.setCursor(0, 0);
    lcd.print("DS SENSOR");
    lcd.setCursor(0, 1);
    lcd.print("NOT FOUND!");
    delay(10000);
  }
  //disabled the wait for conversion on the ds18. It normally adds a 1sec delay
  sensors.setWaitForConversion(false);
}

