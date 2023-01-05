#include "adafruit_io_http.h"


bool postToFeed(String feed, float value) {
  // Build the URL first.
  const String url_str = AIO_FEED_URL + feed + "/data?x-aio-key=" + AIO_KEY;
  const String data_str = "{\"value\": " + (String)value + "}";

  const int post_code = httpPost(url_str, F("application/json"), data_str);
  if (post_code == 200 || post_code == 201) {
    SerialUSB.println("HTTP POST success.");
  } else {
    SerialUSB.println("HTTP POST fail: " + (String)post_code);
  }
}

bool getLastValueFromFeed(String feed) {
  const String url_str = AIO_FEED_URL + feed + "/data?x-aio-key=" + AIO_KEY;

  String response;
  const int get_code = httpGet(url_str, F("application/json"), response);
  if (get_code == 200 || get_code == 201) {
    SerialUSB.println("HTTP GET success.");
  } else {
    SerialUSB.println("HTTP GET fail: " + (String)get_code);
  }
}
