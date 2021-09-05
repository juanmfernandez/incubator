#define UMBRAL 37.70
#define UMBRAL_INF 37.30
#define UMBRAL_SUP 38.00
#define TEMP_LAST_6 36.50
#define TEMP_INF_LAST_6 36.00
#define TEMP_SUP_LAST_6 37.00
#define HUM_PREV_18 60
#define HUM_LAST_6 73
#define HEATER_PIN 9
#define FAN_PIN 10
#define FLIP_EGGS_PIN 11
#include "Incubator.h"

bool dev = true;
bool prod = false;
bool reset = false;
bool flip = true;
float tanterior = 0;

Incubator incubator;


void setup() {
  incubator.relayManage(FLIP_EGGS_PIN, HIGH);
  if(reset){
    incubator.reset();
  }

  pinMode(BUZZER_PIN, OUTPUT);    
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FLIP_EGGS_PIN, OUTPUT);
  
  incubator.setUp();
  incubator.beep();

}

void loop() {
    
    float ds18b20_in = incubator.getCurrentTemp();
    float ds18b20_ext = incubator.getCurrentExtTemp();
    float dht22temp = incubator.getCurrentDHT22Temp();
    float dht22hum = incubator.getCurrentDHT22Hum();
    int mq7co2 = incubator.getCo2();

    if(dev){
      incubator.debug(ds18b20_in, ds18b20_ext, dht22temp, dht22hum, mq7co2);
    }

    incubator.displayStatus(NULL, ds18b20_in, ds18b20_ext, dht22temp, dht22hum, mq7co2);
    
    DateTime now = rtc.now();
    int hour = now.hour();
    int minute = now.minute();
    int day = now.day();
    int temp = 11;
    int flip_hours[] = {0, 3, 6, 9, 12, 14, 15, 18, 20, 21};
      
    if(IsFlip(hour, flip_hours) && minute == temp && flip){
      incubator.relayManage(FLIP_EGGS_PIN, LOW);
      Serial.println("Se estan volteando los huevos.");
      incubator.displayStatus("Volteando...    ");
      incubator.beep();
    }else{
      incubator.relayManage(FLIP_EGGS_PIN, HIGH);
      incubator.notBeep();
    }
    
    int incStartedDay = incubator.getIncStartedDay();
    int currentDayOfCount = EEPROM.read(9);
    int currentDayOfIncub = EEPROM.read(10);
    
    if (currentDayOfCount != day)
    {
      currentDayOfCount = day;
      EEPROM.put(9, currentDayOfCount);
      currentDayOfIncub++;
      EEPROM.put(10, currentDayOfIncub);

      Serial.print("Han pasado ");
      Serial.print(currentDayOfIncub);
      Serial.print(" dias desde el inicio,");
      Serial.print(" ultimo dia contado: ");
      Serial.println(currentDayOfCount);      
    }

    if (currentDayOfIncub >= 18)
    {
      if(ds18b20_in < TEMP_INF_LAST_6){
        incubator.relayManage(HEATER_PIN, LOW);
      }else if(ds18b20_in > TEMP_SUP_LAST_6){
        incubator.notBeep();
        incubator.relayManage(FAN_PIN, LOW);
        incubator.relayManage(HEATER_PIN, HIGH);
      }else{
        incubator.notBeep();
      }
      if (currentDayOfIncub > 21)
      {
        incubator.displayStatus("LOS POLLITOS YA DEBERIAN HABER NACIDO");
      }
      flip = false;
    }else if (currentDayOfIncub < 18){
      if(ds18b20_in < UMBRAL_INF){
        incubator.relayManage(HEATER_PIN, LOW);
      }else if(ds18b20_in > UMBRAL_SUP){
        incubator.notBeep();
        incubator.relayManage(FAN_PIN, LOW);
        incubator.relayManage(HEATER_PIN, HIGH);
      }else{
        incubator.notBeep();
      }
    }
    
}

bool IsFlip(int hour, int flip_hours[])
{
  for (int k = 0; k < 9; k++)
  {
    if (flip_hours[k] == hour)
    {
      return true;
    }
  }
  return false;
}
