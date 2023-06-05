#include "sim7600.h"

/*
 curl -XPOST "***REMOVED***" \
  -H 'Authorization: Token ***REMOVED***:***REMOVED***' \
  --data-raw "temp_rh,location=garaz temp=17,rh=32"
*/

#define USER "***REMOVED***"
#define PASSWORD "***REMOVED***"
#define DB_URL "***REMOVED***"

bool postToDatabase(String feed, const String& key, const float& temperature, const float& humidity, const float& pressure);
