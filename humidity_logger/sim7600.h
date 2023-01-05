
#define SIM7600_RESET_PIN 6
#define SIM7600_PWRKEY_PIN 5
#define SIM7600_FLIGHT_PIN 7


bool initModem();

void powerUp();
void cyclePowerKey(); 

void disableEcho();

void waitUntilModemResponds();
void waitUntilNetworkReady();
void waitUntilGprsReady();

bool sendCommandUntilReplyCorrect(const String& command, const String& reply, int timeout_ms);

bool sendCommandAndCheckReply(const String& command, const String& reply, int timeout_ms);

String sendCommandAndReadReply(const String& command, int timeout_ms);

String replaceCrLfWithText(const String& input);
