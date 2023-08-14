#include "influxdb.h"

bool postToDatabase(String feed, const String& location, const float& temperature, const float& humidity, const float& pressure) {
  // Build the URL first.
  const String url_str = DB_URL;
  const String key_str = (String)F("Authorization: Token ") + USER + ":" + PASSWORD;
  const String data_str = "temp_rh,location=" + location + " temp=" + (String)temperature + ",rh=" + (String)humidity + ",pressure=" + (String)pressure;

  const int post_code = httpPost(url_str, F("application/json"), key_str, data_str);
  if (post_code == 200 || post_code == 201 || post_code == 204) {
    SerialUSB.println("HTTP POST success.");
    return true;
  } else {
    SerialUSB.println("HTTP POST fail: " + (String)post_code);
    return false;
  }
}
