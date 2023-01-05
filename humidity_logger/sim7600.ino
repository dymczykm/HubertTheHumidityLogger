#include "sim7600.h"

bool initModem() {
  powerUp();

  waitUntilModemResponds();
  disableEcho();
  waitUntilNetworkReady();
  waitUntilGprsReady();

  return true;
}

void powerUp() {
  Serial1.begin(115200);
   
  pinMode(SIM7600_RESET_PIN, OUTPUT);
  pinMode(SIM7600_PWRKEY_PIN, OUTPUT);
  pinMode(SIM7600_FLIGHT_PIN, OUTPUT);
    
  digitalWrite(SIM7600_RESET_PIN, LOW);
  digitalWrite(SIM7600_FLIGHT_PIN, LOW);

  delay(100);
  cyclePowerKey();
}

void cyclePowerKey() {
  digitalWrite(SIM7600_PWRKEY_PIN, HIGH);
  delay(2000);
  digitalWrite(SIM7600_PWRKEY_PIN, LOW);
}

void waitUntilModemResponds() {
  bool status = false;
  do {
    for (int i = 0; i < 3 && !status; ++i) {
      status |= sendCommandUntilReplyCorrect(F("AT"), F("AT\n\rOK"), 500, 3);
      // Also support the case when the modem was not restarted and the echo is already turned off.
      status |= sendCommandUntilReplyCorrect(F("AT"), F("OK"), 500, 3);
    }
    if (!status) {
      cyclePowerKey();
    }
  } while (!status);
}

void disableEcho() {
  sendCommandAndCheckReply(F("ATE0"), F("ATE0\nOK"), 500);
}

void waitUntilNetworkReady() {
  sendCommandUntilReplyCorrect(F("AT+CGREG?"), F("+CGREG: 0,1\nOK"), 500, 20);
}

void waitUntilGprsReady() {
  sendCommandUntilReplyCorrect(F("AT+CGATT?"), F("+CGATT: 1\nOK"), 500, 20);
}

int parseHttpActionReply(const String& http_action_reply) {
  if (http_action_reply.length() == 0) {
    SerialUSB.println("Error: empty HTTPACTION response.");
    return false;
  }
  const int first_comma_idx = http_action_reply.indexOf(",");
  const int second_comma_idx = http_action_reply.indexOf(",", first_comma_idx + 1);
  
  String http_code = http_action_reply.substring(first_comma_idx + 1, second_comma_idx);
  return http_code.toInt();  
}

String readHttpResponseData() {
  // Check how many bytes should be read.
  const String reply_bytes_str = sendCommandAndReadReply(F("AT+HTTPREAD?"), 200);

  // Format: +HTTPREAD: LEN,<len>
  if (reply_bytes_str.indexOf("OK") < 0) {
    SerialUSB.println("Error: failed to read HTTP response: " + reply_bytes_str);
    return "";
  }
  
  const int first_idx = reply_bytes_str.indexOf(",");
  const int second_idx = reply_bytes_str.indexOf("\r", first_idx + 1);

  const int bytes_to_read = reply_bytes_str.substring(first_idx + 1, second_idx).toInt();

  const String reply = sendCommandAndReadReply((String)F("AT+HTTPREAD=") + (String)bytes_to_read, 2000);
  return reply;
}

int httpPost(const String& url, const String& content_type, const String& data) {
  const String url_str = (String)F("AT+HTTPPARA=\"URL\",\"") + url + F("\"");
  const String content_str = (String)F("AT+HTTPPARA=\"CONTENT\",\"") + content_type + F("\"");
  
  // This doesn't work, seems to be a bug within SIM7600. It definitely doesn't show up in the outgoing request.
  // String aio_key_str = "AT+HTTPPARA=\"USERDATA\",\"x-aio-key: <key>\""; 
   
  const String data_cmd_str = (String)F("AT+HTTPDATA=") + String(data.length()) + F(",9900");
  
  if (!sendCommandAndCheckReply(F("AT+HTTPINIT"), "OK", 200)) {
    sendCommandAndCheckReply(F("AT+HTTPTERM"), "OK", 200);
    return -1;  
  }
  
  sendCommandAndCheckReply(url_str, "OK", 200);
  sendCommandAndCheckReply(content_str, "OK", 200);
  
  sendCommandAndCheckReply(data_cmd_str, "DOWNLOAD", 200);
  delay(100);
  sendCommandAndCheckReply(data, "OK", 1000);
  
  const String action_response = sendCommandAndReadReply(F("AT+HTTPACTION=1"), 10000);
  
  sendCommandAndCheckReply(F("AT+HTTPTERM"), "OK", 200);
  
  return parseHttpActionReply(action_response);
}

int httpGet(const String& url, const String& content_type, String& response) {
  const String url_str = (String)F("AT+HTTPPARA=\"URL\",\"") + url + F("\"");
  const String content_str = (String)F("AT+HTTPPARA=\"CONTENT\",\"") + content_type + F("\"");

  if (!sendCommandAndCheckReply(F("AT+HTTPINIT"), "OK", 200)) {
    sendCommandAndCheckReply(F("AT+HTTPTERM"), "OK", 200);
    return -1;  
  }
  
  sendCommandAndCheckReply(url_str, "OK", 200);
  sendCommandAndCheckReply(content_str, "OK", 200);

  const String action_response = sendCommandAndReadReply(F("AT+HTTPACTION=0"), 10000);

  response = readHttpResponseData();

  sendCommandAndCheckReply(F("AT+HTTPTERM"), "OK", 200);

  return parseHttpActionReply(action_response);
}



bool sendCommandUntilReplyCorrect(const String& command, const String& reply, int timeout_ms, int max_num_trials) {
  bool result = false;
  int num_trials = 0;
  do {
    delay(1000);
    result = sendCommandAndCheckReply(command, reply, timeout_ms);
    ++num_trials;
  } while (!result && num_trials < max_num_trials);

  return result;
}

void trimString(String& string) {
  string.trim();

  // For some funny corner-case reason we cannot just use: string.replace("\r", "");
  int index = -1;
  do {
    index = string.indexOf("\r");
    if (index > -1) {
      string.remove(index, 1);
    }
  } while (index > -1);

  string.replace("\n\n", "\n");
}

bool sendCommandAndCheckReply(const String& command, const String& expected_reply, int timeout_ms) {
  String reply = sendCommandAndReadReply(command, timeout_ms);

  trimString(reply);

  String expected_reply_trim = expected_reply;
  trimString(expected_reply_trim);

  if (reply == expected_reply_trim) {
    return true;
  } else {
    SerialUSB.println((String)F("Reply does not match the expected one: \"") + replaceCrLfWithText(reply) + F("\" while \"") + replaceCrLfWithText(expected_reply_trim) + F("\" was expected."));
    return false;
  }
}

String sendCommandAndReadReply(const String& command, int timeout_ms) {
  Serial1.println(command + "\r\n");

  String response;
  long int time = millis();
  while ((time + timeout_ms) > millis()) {
    while (Serial1.available()) {
      const char c = Serial1.read();
      response += c;
    }
  }
  if (DEBUG) {
    SerialUSB.print("Sent command: " + command + "\r\n");
    //SerialUSB.print("Got response: \r\n" + response);
  }
  return response;
}

String replaceCrLfWithText(const String& input) {
  String modified_input = input;
  modified_input.replace("\n", "\\n");
  modified_input.replace("\r", "\\r");
  return modified_input;
}
