#include <stdio.h>                                                      //Include the required libraries
#include <string.h>

#include <Seeed_BME280.h>

#include "adafruit_io_http.h"
#include "sim7600.h"



#define DEBUG 1                                                      //Define LTE parameters and pin functions


BME280 bme280;

// CGACT, AT+CSCLK=2 CGATT

void setup()
{
  SerialUSB.begin(115200);
  while (!SerialUSB)
  {
    ; // wait for Arduino serial Monitor port to connect
  }
  
  SerialUSB.println("Hi.");
  
  initModem();
  SerialUSB.println(F("LTE init complete."));

  //    if(!bme280.init()) {
  //      Serial.println("BME280 init error!");
  //    }
  
  SerialUSB.println(F("BME init complete."));
  
  SerialUSB.println(F("Setup complete."));
}

bool getFeed(String feed) {
  String url_str = "AT+HTTPPARA=\"URL\",\"https://io.adafruit.com/api/v2/mdymczyk/feeds/garaz." + feed + "/data?x-aio-key=" + AIO_KEY + "\"\r\n";
  String content_str = "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n";
//  
//  // This doesn't work, seems to be a bug within SIM7600. It definitely doesn't show up in the outgoing request.
//  //String aio_key_str = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: <key>\"\r\n"; 
//  
//  sendData("AT+HTTPINIT\r\n", 2000, DEBUG);
//  
//  sendData(url_str, 1000, DEBUG);
//  sendData(content_str, 1000, DEBUG);
//  
//  String response = sendData("AT+HTTPACTION=0\r\n", 10000, DEBUG);
//
//  sendData("AT+HTTPREAD=0,100\r\n", 500, DEBUG);
//
//  sendData("AT+HTTPTERM\r\n", 500, DEBUG);

  return true;
}

void loop()
{
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
  
  postToFeed("temperature", temperature);
  postToFeed("pressure", pressure_hpa);
  postToFeed("humidity", humidity);

  getLastValueFromFeed("pump");

  delay(5000);
}
