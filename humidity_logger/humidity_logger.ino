#define BMP280_DEBUG_PRINT

#include <Seeed_BME280.h>
//#include <TimeLib.h>
#include "ArduinoLowPower.h"

#include "influxdb.h"
#include "sim7600.h"

#define DEBUG 0

#define RH_VHIGH_PIN 10
#define RH_HIGH_PIN 11
#define RH_OK_PIN 12
#define RH_LOW_PIN 13

#define ACT_LED_PIN 3
#define ERR_LED_PIN 17

BME280 bme280;
//Adafruit_BME280 bme280;

void initIo() {
  pinMode(RH_VHIGH_PIN, OUTPUT); // Very high RH indicator.
  pinMode(RH_HIGH_PIN, OUTPUT); // High RH indicator.
  pinMode(RH_OK_PIN, OUTPUT); // OK RH indicator.
  pinMode(RH_LOW_PIN, OUTPUT); // Low RH indicator.

  pinMode(ACT_LED_PIN, OUTPUT); // ACT.
  pinMode(ERR_LED_PIN, OUTPUT); // ERR.
}

void outputRhLed(const float humidity) {
  // Turn off all LEDs.
  digitalWrite(RH_VHIGH_PIN, 0);
  digitalWrite(RH_HIGH_PIN, 0);
  digitalWrite(RH_OK_PIN, 0);
  digitalWrite(RH_LOW_PIN, 0);

  // Turn on the correct LED.
  if (humidity > 70) {
    digitalWrite(RH_VHIGH_PIN, 1);
  } else if (humidity > 60) {
    digitalWrite(RH_HIGH_PIN, 1);
  } else if (humidity > 30) {
    digitalWrite(RH_OK_PIN, 1);
  } else {
    digitalWrite(RH_LOW_PIN, 1);
  }
}

void setup() {
  SerialUSB.begin(115200);
  while (DEBUG && !SerialUSB)
  {
    ; // wait for Arduino serial Monitor port to connect
  }
  
  SerialUSB.println(F("\n\nHi."));

  initIo();

  digitalWrite(ACT_LED_PIN, 1);
  digitalWrite(ERR_LED_PIN, 1);
  
  initModem();
  SerialUSB.println(F("LTE init complete."));

  if (!bme280.init()) {
    // Hang here forever but it seems the init() routing actually never exits.
    while(1);
  }

  digitalWrite(ACT_LED_PIN, 0);
  digitalWrite(ERR_LED_PIN, 0);
  
  SerialUSB.println(F("Setup complete."));
}

void loop() {
  digitalWrite(ACT_LED_PIN, 0);
  digitalWrite(ERR_LED_PIN, 0);
  
  float temperature = bme280.getTemperature();
  float pressure_hpa = bme280.getPressure() / 100;
  float humidity = bme280.getHumidity(); 

  outputRhLed(humidity);  

  SerialUSB.print("Temp: ");
  SerialUSB.print(temperature);
  SerialUSB.println("C");
  SerialUSB.print("Humidity: ");
  SerialUSB.print(humidity);
  SerialUSB.println("%");
  SerialUSB.print("Pressure: ");
  SerialUSB.print(pressure_hpa);
  SerialUSB.println("hPa");
  
  if (postToDatabase(F("temp_rh"), F("garaz"), temperature, humidity, pressure_hpa)) {
      digitalWrite(ACT_LED_PIN, 1);
  } else {
      digitalWrite(ERR_LED_PIN, 1);
  }
  LowPower.sleep(2000);

  digitalWrite(ACT_LED_PIN, 0);

  // Sleep for 5 minutes.
  LowPower.sleep(300000ul);
}
