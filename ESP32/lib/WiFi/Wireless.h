#include <WiFi.h>
#include <blesmall.h>

bool WiFiScanNodes();
bool WiFiConnect(std::string SSID, std::string Password);
String translateEncryptionType(wifi_auth_mode_t encryptionType);
