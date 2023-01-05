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
  sendCommandUntilReplyCorrect(F("AT"), F("AT\n\rOK"), 500, 20);
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
    SerialUSB.println("Expected response received.");
    return true;
  } else {
    SerialUSB.println("Reply does not match the expected one: \"" + replaceCrLfWithText(reply) + "\" while \"" + replaceCrLfWithText(expected_reply_trim) + "\" was expected.");
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
    SerialUSB.print("Got response: \r\n" + response);
  }
  return response;
}

String replaceCrLfWithText(const String& input) {
  String modified_input = input;
  modified_input.replace("\n", "\\n");
  modified_input.replace("\r", "\\r");
  return modified_input;
}
