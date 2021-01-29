#pragma once
#include <EEPROM.h>

#define EEPROM_UNINITIALIZED_VALUE 0xFF       // Default value is stored in the EEPROM when it is not initialized
#define EEPROM_WIFI_SSID_NAME_LEN_ADDR 0x00   // Address of the flash memory stores length of WiFi SSID Name
#define EEPROM_WIFI_PASSWORD_LEN_ADDR 0x01    // Address of the flash memory stores length of WiFi Password
#define EEPROM_GOOGLE_HOME_NAME_LEN_ADDR 0x02 // Address of the flash memory stores length of Google Home Name
#define EEPROM_AZURE_IOT_HUB_LEN_ADDR 0x03    // Address of the flash memory stores length of Google Home Name
#define EEPROM_STORAGE_START_ADDR 0x0A        // Starting address of the flash memory actual values are stored

enum EEPROM_VALUE
{
    WiFi_SSID,
    WiFi_Password,
    Google_Home_Name,
    Azure_IoT_Hub
};

void EraseFlash(bool safe = false);
void ReadFlash();
void WriteFlash(std::string wifi_ssid, std::string wifi_pass, std::string home_name, std::string iot_hub_connection_string);
void WriteFlashHomeName(std::string home_name);
void WriteAzureIoTHub(std::string connection_string);
void WriteFlashWiFi(std::string wifi_ssid, std::string wifi_pass);
bool HasValidWiFi();
bool HasValidHome();
bool HasValidAzure();

std::string GetFlashValue(EEPROM_VALUE value, bool readFlash = false);
