//MakerFabs Weather Station with 4G
//The DIY Life by Michael Klements
//4 December 2021

#include <stdio.h>                                                      //Include the required libraries
#include <string.h>

#include <Seeed_BME280.h>

String Apikey = "?????";                                                //Thingspeak write API key

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

    if(!bme280.init()) {                                                  //Connect to pressure sensor
      Serial.println("BME280 init error!");
    }

    SerialUSB.println("BME init complete.");
    
    delay(1000);                                                        //Wait 1 second for sensors to start

    SerialUSB.println("Setup complete.");
}

void loop()
{
    float temperature = bme280.getTemperature();
    float pressure_hpa = bme280.getPressure() / 100;
    float humidity = bme280.getHumidity(); 
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

    String http_str = "AT+HTTPPARA=\"URL\",\"https://api.thingspeak.com/update?api_key=" + Apikey + "&field1=" + (String)temperature + "&field2=" + (String)humidity + "&field3=" + (String)pressure_hpa + "\"\r\n";
    //SerialUSB.println(http_str);                                     //Print to serial monitor for debugging

    sendData("AT+HTTPINIT\r\n", 2000, DEBUG);                          //Send the data to Thingspeak
    sendData(http_str, 2000, DEBUG);
    sendData("AT+HTTPACTION=0\r\n", 3000, DEBUG);
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
