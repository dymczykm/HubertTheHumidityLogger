#include "sim7600.h"

#include <ArduinoJson.h>

#define AIO_KEY "aio_JeFc31mpZomKU8KaxpU8QTDlmT5n"
#define AIO_FEED_URL "https://io.adafruit.com/api/v2/mdymczyk/feeds/garaz."

bool postToFeed(String feed, const String& value);
DynamicJsonDocument getLastValueFromFeed(String feed);

DynamicJsonDocument getJsonDocumentFromString(const String& json_string);
