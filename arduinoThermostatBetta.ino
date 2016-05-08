#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
// DS18B20 port / one wire bus
const int ONE_WIRE_BUS = 7;
//relay pin on the board
const int RELAY_PIN = 6;
//update interval for main logic
const long UPDATE_INTERVAL = 2000;
//used to mantain the state machine
long previousMillis = 0;
//target temperature.
const float targetTemp = 25;
//temperature tolerance. The program will try to maintain the temperature beetween the targetTemp +(-) tolerance 
const float TEMP_TOLERANCE = 0.5;
//water sensor offset. it's used to tune the temperature that's being read
const float SENSOR_OFFSET = 0;
// Onewire bus
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//ambient sensor
DeviceAddress sensor1;
//water sensor
DeviceAddress sensor2;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//ambient temperature
float tempAmbient = 0;
//water temperature
float tempWater = 0;
//is the target temperature achieved
bool tempAchieved = true;
//controls the relay state
int rlstate = LOW;
//controls how much time to wait before activating the relay
const long RELAY_GRACETIME = 10000;

void loop()
{
  //To avoid using delay() we use a state machine
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > UPDATE_INTERVAL) {
    previousMillis = currentMillis;
    sensors.requestTemperatures();
    tempAmbient = sensors.getTempC(sensor1);    
    tempWater = sensors.getTempC(sensor2);    

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
    //wait RELAY_GRACETIME time after power up before activating the relay    
    if (currentMillis > RELAY_GRACETIME) {      
      digitalWrite(RELAY_PIN, rlstate);
    }    
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

void setupSensors() {
  //initializes the onewire sensors
  sensors.begin();
  if (!sensors.getAddress(sensor1, 0))
  {
    lcd.setCursor(0, 0);
    lcd.print("VERIFY");
    lcd.setCursor(0, 1);
    lcd.print("AMBIENT SENSOR!");
    delay(10000);
  }

  if (!sensors.getAddress(sensor2, 1))
  {
    lcd.setCursor(0, 0);
    lcd.print("VERIFY");
    lcd.setCursor(0, 1);
    lcd.print("WATER SENSOR!");
    delay(10000);
  }
  //disabled the wait for conversion on the ds18. It normally adds a 1sec delay
  sensors.setWaitForConversion(false);  
}



