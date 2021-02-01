#include <WiFi.h>
#include <blesmall.h>

#define CONFIG_NIMBLE_CPP_ENABLE_RETURN_CODE_TEXT

void WiFiInit(void (*event_queue_method)(CustomEvents));
bool WiFiScanNodes();
bool WiFiConnect(std::string SSID, std::string Password);
String translateEncryptionType(wifi_auth_mode_t encryptionType);
