#include <ArduinoJson.h>
#include <Seeed_BME280.h>
#include <TimeLib.h>

#include "adafruit_io_http.h"
#include "sim7600.h"

#define DEBUG 1
#define PUMP_TIME_SEC 50
#define PUMP_STATE_OFF "\"OFF\""

BME280 bme280;

String last_pump_id;
bool pump_state = false;
time_t turn_on_time;

void handlePump(const DynamicJsonDocument& pump_value) {
  // {"id":"0F6WDMZW7HGER0FK0BS2ZZ086F","value":"OFF","feed_id":2423708,"feed_key":"garaz.pump","created_at":"2023-01-05T16:19:44Z","created_epoch":1672935584,"expiration":"2023-02-04T16:19:44Z"}
  const String pump_value_id = pump_value["id"].as<String>();
  const String feed_value = pump_value["value"].as<String>();

  SerialUSB.println("Pump state: " + (String)pump_state + ", feed state: " + feed_value);

  if (!pump_state && feed_value == "OFF") {
    pump_state = false;
  } else if (!pump_state && feed_value == "ON") {
    // Pump is off, but might be on.
    // Check if this signal has not been used before.
    if (pump_value_id != last_pump_id) {
      // A new trigger, let's turn on the pump.
      pump_state = true;
      last_pump_id = pump_value_id;
      turn_on_time = now();
      SerialUSB.println((String)F("Turned on the pump with feed id value: ") + last_pump_id);
    } else {
      // This trigger was used already. Let's send an off value to the feed.
      SerialUSB.println("The feed id value " + last_pump_id + " was used before. Sending OFF to the feed.");
      postToFeed("pump", PUMP_STATE_OFF);
    }
  } else if (pump_state && feed_value == PUMP_STATE_OFF) {
    // Pump is on, but the feed says off. Let's turn it off.
    pump_state = false;    
  } else if (pump_state && feed_value == "ON") {
    // Pump is on and the feed says on. Let's check if the timeout has been reached.
    if (numberOfSeconds(now() - turn_on_time) > PUMP_TIME_SEC) {
      SerialUSB.println(F("The time threshold elapsed, turning off the pump."));
      pump_state = false;
      postToFeed("pump", PUMP_STATE_OFF);
    } else {
      SerialUSB.println((String)F("Pump time: ") + (String)((int)numberOfSeconds(now() - turn_on_time)) + F(" secs"));
    }
  } else {
    SerialUSB.println(F("Error while handling the pump state."));
    pump_state = false;
  }

  // Control the pump according to the state.
  SerialUSB.println((String)F("Final pump state: ") + (String)pump_state);
}

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

  pump_state = false;
  last_pump_id = "";
  
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
  
  //postToFeed("temperature", temperature);
  //postToFeed("pressure", pressure_hpa);
  postToFeed("humidity", (arduino::String)humidity);

//  const DynamicJsonDocument feed_value = getLastValueFromFeed("pump"); 
//  if (!feed_value.isNull()) {
//    handlePump(feed_value);
//  }

  delay(1000);
}
