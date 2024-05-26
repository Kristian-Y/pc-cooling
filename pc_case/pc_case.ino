#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

// DB18B20 one wire in take temp sensor
// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 7
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
float intakeTemp = 0;

// DHT 11 
#define DHT11_PIN 8
DHT dht11(DHT11_PIN, DHT11);
float dhtTemp = 0;
float dhtHumadity = 0;


// Dust sensor
int measurePin = A0; //Connect dust sensor to Arduino A0 pin
int ledPower = 13;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

byte dustChar[8] = {
	0b00000,
	0b00000,
	0b11011,
	0b01110,
	0b01110,
	0b11011,
	0b00000,
	0b00000
};

// lcd display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// fans
int rpm = 0;
int count = 0;
unsigned long start_time;

void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  pinMode(6,INPUT);
  dht11.begin();
  sensors.begin();
  lcd.begin(16, 2);
  lcd.createChar(0, dustChar);
  attachInterrupt(digitalPinToInterrupt(6), counter, RISING);
}

void loop(){
  count = 0;
  start_time = millis();
  while(millis() - start_time < 1000) {

  }
  readDustDensity();
  readDHTTemp();
  readIntakeTemp();
  updateScreen();
  float tempSub = (dhtTemp - intakeTemp);
  if(tempSub > 5) {
    analogWrite(9, 90);
  } 
  if(tempSub > 10) {
    analogWrite(9, 129);
  } 
   if(tempSub > 15) {
    analogWrite(9, 200);
  }
   if(tempSub > 20) {
    analogWrite(9, 255);
  } 
  if(tempSub < 5) {
    analogWrite(9, 0);
  }
  delay(1000);
}

void readDustDensity() {
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);

  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 170 * calcVoltage - 0.1;

  
 Serial.println(dustDensity); // unit: ug/m3
}

void readDHTTemp() {
    // read humidity
  dhtHumadity  = dht11.readHumidity();
  // read temperature as Celsius
  dhtTemp = dht11.readTemperature();
}

void readIntakeTemp() {
  sensors.requestTemperatures();
  intakeTemp = sensors.getTempCByIndex(0); 
}

void updateScreen() {
  
  
  lcd.clear();
  lcd.print(dhtTemp);
  
  if(dustDensity > 200) {
    lcd.setCursor(7, 1);
    lcd.write((byte)0);
  }
  Serial.println(intakeTemp);
  lcd.setCursor(0, 1);
  lcd.print(intakeTemp);

}

void counter() {
  count++;
}
