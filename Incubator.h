#include "Arduino.h"
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <DHT_U.h>
#include "RTClib.h"
#include "EEPROM.h"

#define BUZZER_PIN 7
#define ONE_WIRE_BUS 8
#define DHT22_PIN 6
#define MQ_7_PIN A0
#define co2Zero 55

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
DHT dht22(DHT22_PIN, DHT22);

RTC_DS1307 rtc;
Servo servo;

int PINSERVO = 6;
int PULSOMIN = 1000;
int PULSOMAX = 2000;

int analog_mq7;

int current_temp;
int current_ext_temp;

int current_dht22_temp;
int current_dht22_humidity;

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7);

class Incubator {

    private:

    public:

        void setUp(){

            Serial.begin(9600);

            servo.attach(PINSERVO, PULSOMIN, PULSOMAX);
            servo.write(0);
            
            ds18b20.begin();
            ds18b20.setResolution(0, 12);

            dht22.begin();

            pinMode(BUZZER_PIN, OUTPUT);
            //relayManage(FLIP_EGGS_PIN, HIGH);
            
            if(! rtc.begin() ){
              Serial.print("No se encontro el RTC1307");
            }
            if(! rtc.isrunning() ){
              Serial.print("No se inicializo el RTC1307");
            }
            //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

            int dir0 = EEPROM.read(0);
            int year = 2021;
            
            if(dir0 == 255){
              DateTime setTime = rtc.now();
              year = setTime.year();              
              EEPROM.put(0, setTime.hour());
              EEPROM.put(1, setTime.minute());
              EEPROM.put(2, setTime.day());
              EEPROM.put(3, setTime.month());
              EEPROM.put(4, year);
            }else{
              Serial.print("La incubacion comenzo a las  ");
              Serial.print(EEPROM.read(0));
              Serial.print(":");
              Serial.print(EEPROM.read(1));
              Serial.print(" del  ");
              Serial.print(EEPROM.read(2));
              Serial.print("/");
              Serial.print(EEPROM.read(3));
              Serial.print("/");
              Serial.println(EEPROM.get(4, year));
            }

            lcd.setBacklightPin(3, POSITIVE);
            lcd.setBacklight(HIGH);
            lcd.begin(16, 2);
            lcd.clear();
        }
    
        float getCurrentTemp() {
          ds18b20.requestTemperatures();
          return(ds18b20.getTempCByIndex(0));
        }

        float getCurrentExtTemp() {
          ds18b20.requestTemperatures();
          return(ds18b20.getTempCByIndex(1));
        }
        
        float getCurrentDHT22Hum() {
          current_dht22_temp = dht22.readHumidity();
          return(current_dht22_temp);
        }

        float getCurrentDHT22Temp() {
          current_dht22_humidity = dht22.readTemperature();
          return(current_dht22_humidity);
        }
        
        int getCo2(){
          int co2now[10];                               //int array for co2 readings
          int co2raw = 0;                               //int for raw value of co2
          int co2comp = 0;                              //int for compensated co2 
          int co2ppm = 0;                               //int for calculated ppm
          int zzz = 0;                                  //int for averaging

          for (int x = 0;x<10;x++){                     //samplpe co2 10x over 2 seconds
            co2now[x]=analogRead(MQ_7_PIN);
            delay(200);
          }

          for (int x = 0;x<10;x++){                     //add samples together
            zzz=zzz + co2now[x];    
          }
          
          co2raw = zzz/10;                              //divide samples by 10
          co2comp = co2raw - co2Zero;                   //get compensated value
          co2ppm = map(co2comp,0,1023,400,5000);        //map value for atmospheric levels
          return co2ppm;
        }

        void beep(){
          digitalWrite(BUZZER_PIN, HIGH);
          delay(100);
          digitalWrite(BUZZER_PIN, LOW);
        }

        void notBeep(){
          digitalWrite(BUZZER_PIN, LOW);
        }
        
        void open(){
          //servo.write(180);
        }

        void close(){
          //servo.write(0);
        }
        
        int getServoStatus(){
          return servo.read();
        }

        void displayStatus(char msg[] = NULL, float ds18b20_in = NULL, float ds18b20_ext = NULL, float dht22temp = NULL, float dht22hum = NULL, int mq7co2 = NULL) {
            if(msg == NULL){
              lcd.setCursor(0, 0);
              lcd.print("Temp  ");
              lcd.setCursor(6, 0);
              lcd.print("Hum   ");
              lcd.setCursor(12, 0);
              lcd.print("Co2 ");

              lcd.setCursor(0, 1);
              lcd.print(ds18b20_in);
              
              lcd.setCursor(6, 1);
              if(dht22hum == 0.000){
                lcd.print("Error ");  
              }else{
                lcd.print(dht22hum);
              }              
              
              lcd.setCursor(12, 1);
              lcd.print(mq7co2);
              
            }else{
              lcd.setCursor(0, 0);
              lcd.print(msg);
            }        
        }
        
        void relayManage(int PIN, bool state){
          digitalWrite(PIN, state);
        }

        int getIncStartedDay(){
          return EEPROM.read(2);
        }

        void reset(){
          EEPROM.write(0, 255);
          EEPROM.write(9, 255);
          EEPROM.write(10, 0);
        }


        void debug(float ds18b20_in = NULL, float ds18b20_ext = NULL, float dht22temp = NULL, float dht22hum = NULL, int mq7co2 = NULL){
          DateTime now = rtc.now();
          Serial.print(now.hour());
          Serial.print(":");
          Serial.print(now.minute());
          Serial.print(":");
          Serial.print(now.second());
          Serial.print(" - ");
          Serial.print(now.day());
          Serial.print("/");
          Serial.print(now.month());
          Serial.print("/");
          Serial.println(now.year());
          Serial.print("DS18B20_current_int_temp: ");
          Serial.print(ds18b20_in);
          Serial.print(" DS18B20_current_ext_temp: ");
          Serial.print(ds18b20_ext);
          Serial.println(" C");    
          Serial.print("DHT22_temp: ");
          Serial.print(dht22temp);
          Serial.print(" C");
          Serial.print(" DHT22_hum: ");
          Serial.print(dht22hum);
          Serial.print(" %");
          Serial.print(" Co2: ");
          Serial.print(mq7co2);
          Serial.println(" PPM");

        }
        
};
