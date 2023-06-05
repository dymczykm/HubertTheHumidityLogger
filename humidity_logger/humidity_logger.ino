#include <Seeed_BME280.h>
#include <TimeLib.h>

#include "influxdb.h"
#include "sim7600.h"

#define DEBUG 1

BME280 bme280;

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB)
  {
    ; // wait for Arduino serial Monitor port to connect
  }
  
  SerialUSB.println(F("\n\nHi."));
  
  initModem();
  SerialUSB.println(F("LTE init complete."));

  //    if(!bme280.init()) {
  //      Serial.println("BME280 init error!");
  //    }
  
  SerialUSB.println(F("BME init complete."));
  
  SerialUSB.println(F("Setup complete."));

//  pinMode(2, OUTPUT);
//  pinMode(3, OUTPUT);
//  pinMode(10, OUTPUT);
//  pinMode(11, OUTPUT);
//  pinMode(12, OUTPUT);
//  pinMode(13, OUTPUT);
//  pinMode(18, OUTPUT);
//  pinMode(19, OUTPUT);
//
//  while(1) {
//    digitalWrite(2,  !digitalRead(2));
//    digitalWrite(3,  !digitalRead(3));
//    digitalWrite(10, !digitalRead(10));
//    digitalWrite(11, !digitalRead(11));
//    digitalWrite(12, !digitalRead(12));
//    digitalWrite(13, !digitalRead(13));
//    digitalWrite(18, !digitalRead(18));
//    digitalWrite(19, !digitalRead(19));
//    delay(1);
//  }
}

void loop() {
  float temperature = 0; //bme280.getTemperature();
  float pressure_hpa = 0; //bme280.getPressure() / 100;
  float humidity = 0; //bme280.getHumidity(); 
  temperature = 21;
  pressure_hpa = 1000;
  humidity = 55;
  SerialUSB.print("Temp: ");
  SerialUSB.print(temperature);
  SerialUSB.println("C");
  SerialUSB.print("Humidity: ");
  SerialUSB.print(humidity);
  SerialUSB.println("%");
  SerialUSB.print("Pressure: ");
  SerialUSB.print(pressure_hpa);
  SerialUSB.println("hPa");
  
  postToDatabase(F("temp_rh"), F("garaz"), temperature, humidity, pressure_hpa);

  delay(1000);
}
