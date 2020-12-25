#include <WiFi.h>
#include <ble.h>

bool WiFiScanNodes();
bool WiFiConnect(std::string SSID, std::string Password);
String translateEncryptionType(wifi_auth_mode_t encryptionType);
