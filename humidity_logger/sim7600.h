
#define SIM7600_RESET_PIN 6
#define SIM7600_PWRKEY_PIN 5
#define SIM7600_FLIGHT_PIN 7


bool initModem();
int httpPost(const String& url, const String& content_type, const String& user_data, const String& data);
int httpGet(const String& url, const String& content_type, const String& user_data, String& response);

void powerUp();
void cyclePowerKey(); 
void disableEcho();

void waitUntilModemResponds();
void waitUntilNetworkReady();
void waitUntilGprsReady();

bool sendCommandUntilReplyCorrect(const String& command, const String& reply, int timeout_ms);
bool sendCommandAndCheckReply(const String& command, const String& reply, int timeout_ms);
String sendCommandAndReadReply(const String& command, int timeout_ms);
int parseHttpActionReply(const String& http_action_reply);
String replaceCrLfWithText(const String& input);
