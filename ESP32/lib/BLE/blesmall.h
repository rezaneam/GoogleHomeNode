#include <Arduino.h>
#include <StoreHub.h>
#include <stdio.h>
#include <string.h>
#include <NimBLEDevice.h>
#include <sstream>
#include <enums.h>

#define BLE_ADVERTISE_TIMEOUT_S 60                              // BLE advertising timeout
#define BLE_ADVERTISE_TIMEOUT_MS 1000 * BLE_ADVERTISE_TIMEOUT_S // BLE advertising timeout

#define SERVICE_UUID_DEVICE_INFORMATION 0x180A   // Device Information
#define SERVICE_UUID_ENVIROMENTAL_SENSING 0x181A // Environmental Sensing - Keeping BME280 Sensor data
#define SERVICE_UUID_AUTOMATION_IO 0x1815        // Automation IO - Keeping WIFI + Google Home info
#define SERVICE_UUID_USER_DATA 0x181C            // User Data
#define SERVICE_UUID_BINARY_SENSOR 0x183B        // Binary Sensor
#define SERVICE_UUID_BATTERY 0x180F              // Battery Service

#define CHARACTERISTIC_UUID_SYSTEM_ID 0x2A23
#define CHARACTERISTIC_UUID_MANUFACTURER 0x2A29
#define CHARACTERISTIC_UUID_MODEL_NUMBER 0x2A24
#define CHARACTERISTIC_UUID_FIRMWARE_REVISION 0x2A26
#define CHARACTERISTIC_UUID_HARDWARE_REVISION 0x2A27
#define CHARACTERISTIC_UUID_SOFTWARE_REVISION 0x2A28
#define CHARACTERISTIC_UUID_BATTERY_LEVEL 0x2A19      // Battery level
#define CHARACTERISTIC_UUID_PRESSURE 0x2A6D           // Pressure Measurements
#define CHARACTERISTIC_UUID_TEMPERATURE 0x2A6E        // Temperature Measurements
#define CHARACTERISTIC_UUID_HUMIDITY 0x2A6F           // Humidity Measurements
#define CHARACTERISTIC_UUID_RESET_CONFIG 0x4900       // factory reseting the configurations - set 1 to reset ESP and 2 to erase EEPROM and then reset 3  to safe erase EEPROM and then reset.
#define CHARACTERISTIC_UUID_WIFI_SSID_NAMES 0x4901    // Found WiFi names
#define CHARACTERISTIC_UUID_WIFI_SCANNING 0x4902      // Scan Refresh - used for is WiFi scanning: 0 - Not scanning, 1 - scanning
#define CHARACTERISTIC_UUID_WIFI_SSID 0x4903          // WiFi SSID Name
#define CHARACTERISTIC_UUID_WIFI_PASS 0x4904          // WiFi SSID Password
#define CHARACTERISTIC_UUID_CONNECTION_STAT 0x4905    // Connection status: each char for WiFi, Google Home, Azure=> 0 Not connected, 1 (try)Connecting, 2 Connected, 3 unknown
#define CHARACTERISTIC_UUID_GOOGLE_HOME_NAME 0x4906   // Google Home name seats here
#define CHARACTERISTIC_UUID_AZURE_IOT_HUB_CONN 0x4907 // Azure IoT Hub connection string seats here
#define CHARACTERISTIC_UUID_DEVICE_LOCATION 0x4908    // Device Location

#define DESCRIPTOR_UUID_BATTERY 0x3900     // Discriptor for battery level
#define DESCRIPTOR_UUID_PRESSURE 0x3901    // Discriptor for Pressure Sensor
#define DESCRIPTOR_UUID_TEMPERATURE 0x3902 // Discriptor for Temperature Sensor
#define DESCRIPTOR_UUID_HUMIDITY 0x3903    // Discriptor for Humidity Sensor
#define DESCRIPTOR_UUID_WIFI_SCAN 0x3904   // Discriptor for WiFi scan status
#define DESCRIPTOR_UUID_WIFI_SSIDs 0x3905  // Discriptor for for found SSIDs
#define DESCRIPTOR_UUID_WIFI_CONN 0x3906   // Discriptor for WiFi connection status
#define DESCRIPTOR_UUID_GLHM_NAME 0x3907   // Discriptor for Google Home name

#define DESCRIPTOR_VAL_BATTERY "Battery Level"
#define DESCRIPTOR_VAL_TEMPEATURE "Temperature in centigrade scale"
#define DESCRIPTOR_VAL_HUMIDITY "Humidity in percentage(%) scale"
#define DESCRIPTOR_VAL_PRESSURE "Pressure in Pascal scale"
#define DESCRIPTOR_VAL_WIFI_SCAN "WiFi scanning: 0 - Not scanning, 1 - scanning"
#define DESCRIPTOR_VAL_WIFI_SSIDs "List of the SSIDs found after scan"
#define DESCRIPTOR_VAL_WIFI_CONN "Connection status: 0 Not connected, 1 (try)Connecting, 2 Connected"
#define DESCRIPTOR_VAL_GLHM_NAME "Google Home Name"

#define BLE_WIFI_SCANNING_ACTIVE "1"
#define BLE_WIFI_SCANNING_DEACTIVE "0"
#define BLE_NOT_CONNECTED "000"
#define BLE_WIFI_CONNECTING "1"
#define BLE_WIFI_CONNECTED "2"

#define BLE_DEVICE_NAME "Google Home Node - RSv1"
#define BLE_SYSTEM_ID "Samira"
#define BLE_MANUFACTURER "Espressif Systems"
#define BLE_MODEL_NUMBER "Home Sensor"
#define BLE_FIRMWARE_REVISION __DATE__
#define BLE_HARDWARE_REVISION "v1"
#define BLE_SOFTWARE_REVISION "v1"
#define BLE_WIFI_PASS_WRITE_ONLY "WRITE ONLY"

void BLEinit(std::string deviceName, void (*event_queue_method)(CustomEvents), bool verbose = false);
void BLEsetupAd();
void BLEstartAd();
void BLEstopAd();
void addCharacteristic(BLEService *pService, int uuid, uint32_t properties, std::string value, int descriptorUuid = -1, std::string descriptorValue = "");
void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value);
void BLEsetSSIDs(std::string SSIDs);
void BLEsetSSID(std::string value);
void BLEsetGoogleHomeName(std::string value);
bool BLEcanStopAdv();
void BLEupdateConnectionStatus(bool isWiFiConnected, bool isGoogleHomeConnected, bool isAzureConnected);
std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid);
std::string BLEgetSSIDs();
std::string BLEgetSSID();
std::string BLEgetPassword();
void UpdateSensorValues(float temperature, float humidity, float pressure);
std::string convertToString(float value);
bool hasNotifier(BLEUUID uuid);
