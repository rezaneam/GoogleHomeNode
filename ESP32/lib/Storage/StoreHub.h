#pragma once
#include <EEPROM.h>

#define EEPROM_UNINITIALIZED_VALUE 0xFF       // Default value is stored in the EEPROM when it is not initialized
#define EEPROM_WIFI_SSID_NAME_LEN_ADDR 0x00   // Address of the flash memory stores length of WiFi SSID Name
#define EEPROM_WIFI_PASSWORD_LEN_ADDR 0x01    // Address of the flash memory stores length of WiFi Password
#define EEPROM_GOOGLE_HOME_NAME_LEN_ADDR 0x02 // Address of the flash memory stores length of Google Home Name
#define EEPROM_AZURE_IOT_HUB_LEN_ADDR 0x03    // Address of the flash memory stores length of Google Home Name
#define EEPROM_DEVICE_LOCATION_LEN_ADDR 0x04  // Address of the flash memory stores length of Google Home Name
#define EEPROM_USER_NAME_LEN_ADDR 0x05        // Address of the flash memory stores length of Google Home Name
#define EEPROM_BSEC_STAT_LEN_ADDR 0x09        // Address of the flash memory stores length of BSEC Status for BME680 Sensor
#define EEPROM_STORAGE_START_ADDR 0x0A        // Starting address of the flash memory actual values are stored

enum EEPROM_VALUE
{
    WiFi_SSID,
    WiFi_Password,
    Google_Home_Name,
    Azure_IoT_Hub,
    Device_Location,
    User_Name
};

void EraseFlash(bool safe = false);
void ReadFlash();
void WriteFlash(
    std::string wifi_ssid,
    std::string wifi_pass,
    std::string home_name,
    std::string iot_hub_connection_string,
    std::string device_location,
    std::string username);
void WriteFlashBSECstate(uint8_t *state, uint8_t len);
void WriteFlashUsername(std::string username);
void WriteFlashDeviceLocation(std::string device_location);
void WriteFlashHomeName(std::string home_name);
void WriteFlashAzureIoTHub(std::string connection_string);
void WriteFlashWiFi(std::string wifi_ssid, std::string wifi_pass);
bool HasValidWiFi();
bool HasValidHome();
bool HasValidAzure();
bool HasUsername();
bool HasDeviceLocation();
bool HasBSECstate();
bool GetBSECstate(uint8_t *state, uint8_t expectedLength);
std::string GetFlashValue(EEPROM_VALUE value, bool readFlash = false);
