//MakerFabs Weather Station with 4G
//The DIY Life by Michael Klements
//4 December 2021

#include <stdio.h>                                                      //Include the required libraries
#include <string.h>

#include <Seeed_BME280.h>

String adafruit_io_key = "aio_JeFc31mpZomKU8KaxpU8QTDlmT5n";

#define DEBUG true                                                      //Define LTE parameters and pin functions
#define LTE_RESET_PIN 6
#define LTE_PWRKEY_PIN 5
#define LTE_FLIGHT_PIN 7

BME280 bme280;

void setup()
{
    SerialUSB.begin(115200);
    while (!SerialUSB)
    {
      ; // wait for Arduino serial Monitor port to connect
    }
    Serial1.begin(115200);
    
    SerialUSB.println("Init");
  
    pinMode(LTE_RESET_PIN, OUTPUT);
    digitalWrite(LTE_RESET_PIN, LOW);
    pinMode(LTE_PWRKEY_PIN, OUTPUT);
    digitalWrite(LTE_RESET_PIN, LOW);
    delay(100);
    digitalWrite(LTE_PWRKEY_PIN, HIGH);
    delay(2000);
    digitalWrite(LTE_PWRKEY_PIN, LOW);
    pinMode(LTE_FLIGHT_PIN, OUTPUT);
    digitalWrite(LTE_FLIGHT_PIN, LOW);

    delay(1000);
    
    sendData("AT+CRESET\r\n", 500, DEBUG);
    delay(10000);

    String status_response;
    int status_code = -1;
    do {
      delay(1000);
      status_response = sendData("AT+CGREG?\r\n", 500, DEBUG);
      const int comma_idx = status_response.indexOf(",");
      status_code = status_response.substring(comma_idx + 1, comma_idx + 2).toInt();
      SerialUSB.println("CGREG status: " + (String)status_code);
    } while (status_code != 1);
    
    SerialUSB.println("LTE init complete.");

//    if(!bme280.init()) {
//      Serial.println("BME280 init error!");
//    }

    SerialUSB.println("BME init complete.");

    delay(1000);
    
    SerialUSB.println("Setup complete.");
}

bool postToFeed(String feed, float value) {
  String url_str = "AT+HTTPPARA=\"URL\",\"https://io.adafruit.com/api/v2/mdymczyk/feeds/garaz." + feed + "/data?x-aio-key=" + adafruit_io_key + "\"\r\n";
  String content_str = "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n";
  
  // This doesn't work, seems to be a bug within SIM7600. It definitely doesn't show up in the outgoing request.
  //String aio_key_str = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: <key>\"\r\n"; 
  
  String data_json = "{\"value\": " + (String)value + ", \"lat\": 23.1, \"lon\": \"-73.3\"}";
  
  String data_cmd_str = "AT+HTTPDATA=" + String(data_json.length()) + ",9900\r\n";
  
  sendData("AT+HTTPINIT\r\n", 2000, DEBUG);
  
  sendData(url_str, 1000, DEBUG);
  sendData(content_str, 1000, DEBUG);
  
  sendData(data_cmd_str, 500, DEBUG);
  delay(100);
  sendData(data_json, 1000, DEBUG);
  
  String response = sendData("AT+HTTPACTION=1\r\n", 10000, DEBUG);
  if (response.length() == 0) {
    SerialUSB.println("Error parsing the response.");
    return false;
  }
  const int first_comma_idx = response.indexOf(",");
  const int second_comma_idx = response.indexOf(",", first_comma_idx + 1);
  
  String http_code = response.substring(first_comma_idx + 1, second_comma_idx);
  SerialUSB.println("Response code: " + http_code);
  
  sendData("AT+HTTPTERM\r\n", 500, DEBUG);

  if (http_code == "200") {
    return true;
  } else {
    SerialUSB.println("Failed: " + response);
    return false;
  }
}

bool getFeed(String feed) {
  String url_str = "AT+HTTPPARA=\"URL\",\"https://io.adafruit.com/api/v2/mdymczyk/feeds/garaz." + feed + "/data?x-aio-key=" + adafruit_io_key + "\"\r\n";
  String content_str = "AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n";
  
  // This doesn't work, seems to be a bug within SIM7600. It definitely doesn't show up in the outgoing request.
  //String aio_key_str = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: <key>\"\r\n"; 
  
  sendData("AT+HTTPINIT\r\n", 2000, DEBUG);
  
  sendData(url_str, 1000, DEBUG);
  sendData(content_str, 1000, DEBUG);
  
  String response = sendData("AT+HTTPACTION=0\r\n", 10000, DEBUG);

  sendData("AT+HTTPREAD=0,100\r\n", 500, DEBUG);

  sendData("AT+HTTPTERM\r\n", 500, DEBUG);

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

  getFeed("pump");

  delay(5000);
}

String sendData(String command, const int timeout, boolean debug)       //Function to send data over 4G
{
  String response = "";
  Serial1.println(command);
  
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug)
  {
    SerialUSB.print(response);
  }
  return response;
}
