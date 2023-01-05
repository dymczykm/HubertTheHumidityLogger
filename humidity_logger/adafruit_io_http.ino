#include "adafruit_io_http.h"

#include <ArduinoJson.h>

bool postToFeed(String feed, const String& value) {
  // Build the URL first.
  const String url_str = AIO_FEED_URL + feed + "/data?x-aio-key=" + AIO_KEY;
  const String data_str = "{\"value\": " + value + "}";

  const int post_code = httpPost(url_str, F("application/json"), data_str);
  if (post_code == 200 || post_code == 201) {
    SerialUSB.println("HTTP POST success.");
    return true;
  } else {
    SerialUSB.println("HTTP POST fail: " + (String)post_code);
    return false;
  }
}

DynamicJsonDocument getLastValueFromFeed(String feed) {
  const String url_str = AIO_FEED_URL + feed + "/data?x-aio-key=" + AIO_KEY + "&limit=1";

  String response;
  const int get_code = httpGet(url_str, F("application/json"), response);
  if (get_code == 200 || get_code == 201) {
    SerialUSB.println("HTTP GET success.");

    const int first_idx = response.indexOf("[");
    const int second_idx = response.indexOf("]", first_idx + 1);

    return getJsonDocumentFromString(response.substring(first_idx + 1, second_idx));
  } else {
    SerialUSB.println("HTTP GET fail: " + (String)get_code);
    return DynamicJsonDocument(0u);
  }
}


DynamicJsonDocument getJsonDocumentFromString(const String& json_string) {
  // Capacity calculated using an online tool of the library.
  DynamicJsonDocument doc(384u);

  DeserializationError error = deserializeJson(doc, json_string);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return DynamicJsonDocument(0u);
  }

  return doc;
}
