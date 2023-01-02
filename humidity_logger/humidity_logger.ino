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

//#define Serial SerialUSB

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
  
    pinMode(LTE_RESET_PIN, OUTPUT);                                     //Set up LTE pin modes and initial states
    digitalWrite(LTE_RESET_PIN, LOW);
    pinMode(LTE_PWRKEY_PIN, OUTPUT);
    digitalWrite(LTE_RESET_PIN, LOW);
    delay(100);
    digitalWrite(LTE_PWRKEY_PIN, HIGH);
    delay(2000);
    digitalWrite(LTE_PWRKEY_PIN, LOW);
    pinMode(LTE_FLIGHT_PIN, OUTPUT);
    digitalWrite(LTE_FLIGHT_PIN, LOW);                                 //Normal Mode
    
    SerialUSB.println("LTE init complete.");
    
    delay(5000);                                                        //Wait 5 seconds

//    if(!bme280.init()) {                                                  //Connect to pressure sensor
//      Serial.println("BME280 init error!");
//    }

    SerialUSB.println("BME init complete.");
    
    delay(10000);                                                        //Wait 1 second for sensors to start

    SerialUSB.println("Setup complete.");

    sendData("ATI\r\n", 500, DEBUG);

    sendData("AT+CGMR\r\n", 500, DEBUG);

    sendData("AT+HTTPTERM\r\n", 500, DEBUG);

    sendData("AT+CGSOCKCONT=1,\"IP\",\"internet\"\r\n", 500, DEBUG);
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

    //String http_str = "AT+HTTPPARA=\"URL\",\"https://api.thingspeak.com/update?api_key=" + Apikey + "&field1=" + (String)temperature + "&field2=" + (String)humidity + "&field3=" + (String)pressure_hpa + "\"\r\n";
    //SerialUSB.println(http_str);                                     //Print to serial monitor for debugging

    //String http_str = "AT+HTTPPARA=\"URL\",\"https://io.adafruit.com/api/v2/mdymczyk/feeds/temperature/data\"\r\n";
    String http_str = "AT+HTTPPARA=\"URL\",\"https://cvgdg.requestcatcher.com/test\"\r\n";
    //String http_str2 = "AT+HTTPPARA=\"CONTENT\",\"application/json\r\naaa: bbb\"\r\n";
    //String http_str3 = "AT+HTTPPARA=\"ACCEPT\",\"*/*\"\r\n";
    //String http_str35 = "AT+HTTPPARA=\"UA\",\"curl/7.79.1\"\r\n";
    //String http_str4 = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: aio_aaaaaaaaaaaaaaa\"\r\n"; //aio_JeFc31mpZomKU8KaxpU8QTDlmT5n\"\r\n";
    //String http_str41 = "AT+HTTPPARA=\"USERDATA\",\"http_acceptsddss: */*\"\r\n";
    //String http_str4 = "AT+HTTPPARA=\"USERDATA\",\"Authorization:JWT " + (String)"aaa\\r\\n" + (String)"\"" + (String)"\r\n";
    String http_str4 = "AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic Y2FycGx1c2dvOmNhcnBsdXgz\"\r\n";

    //String http_str4 = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: 27\"\r\n";
    
    //String http_str2 = "AT+HTTPPARA=\"CONTENT\",\"text/plain\"\r\n";
    
    String data_json = "{\"value\": 39, \"lat\": 23.1, \"lon\": \"-73.3\"}";
    String data_plain = "\"value=42\"";

    String data = data_json;
    
    String http_str5 = "AT+HTTPDATA=" + String(data.length()) + ",9900\r\n";

    sendData("AT+HTTPINIT\r\n", 2000, DEBUG);

    sendData(http_str, 1000, DEBUG);

//    // GET
//    sendData(http_str3, 1000, DEBUG);
//    //sendData(http_str41, 1000, DEBUG);
//    sendData(http_str4, 1000, DEBUG);
//    sendData("AT+HTTPACTION=0\r\n", 5000, DEBUG);
//    sendData("AT+HTTPHEAD\r\n", 5000, DEBUG);
//    sendData("AT+HTTPREAD=0,200\r\n", 5000, DEBUG);

        
    //sendData(http_str2, 500, DEBUG);
    //sendData(http_str3, 500, DEBUG);
    //sendData(http_str35, 500, DEBUG);

    sendData(http_str4, 2000, DEBUG);

    //sendData(http_str41, 500, DEBUG);

    sendData(http_str5, 500, DEBUG);
    delay(100);
    sendData(data, 2000, DEBUG);
    
    sendData("AT+HTTPACTION=1\r\n", 10000, DEBUG);

    sendData("AT+HTTPHEAD\r\n", 5000, DEBUG);
    sendData("AT+HTTPREAD=0,200\r\n", 5000, DEBUG);

    
    sendData("AT+HTTPTERM\r\n", 3000, DEBUG);

    delay(5000);                                                      //Wait 120 seconds
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
