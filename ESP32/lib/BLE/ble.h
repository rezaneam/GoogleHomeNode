#include <Arduino.h>
#include <stdio.h>
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h> //Library to use BLE as server
#include <BLE2902.h>

#define BLE_ADVERTISE_TIMEOUT 60 // BLE advertising timeout

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
#define CHARACTERISTIC_UUID_BATTERY_LEVEL 0x2A19        // Battery level
#define CHARACTERISTIC_UUID_PRESSURE 0x2A6D             // Pressure Measurements
#define CHARACTERISTIC_UUID_TEMPERATURE 0x2A6E          // Temperature Measurements
#define CHARACTERISTIC_UUID_HUMIDITY 0x2A6F             // Humidity Measurements
#define CHARACTERISTIC_UUID_WIFI_SSID_NAMES 0x2900      // Found WiFi names
#define CHARACTERISTIC_UUID_WIFI_SCANNING 0x2901        // Scan Refresh - used for is WiFi scanning: 0 - Not scanning, 1 - scanning
#define CHARACTERISTIC_UUID_WIFI_SSID 0x2902            // WiFi SSID Name
#define CHARACTERISTIC_UUID_WIFI_PASS 0x2903            // WiFi SSID Password
#define CHARACTERISTIC_UUID_WIFI_CONNECTION_STAT 0x2904 // WiFi Connection status: 0 Not connected, 1 Connecting, 2 Connected

#define BLE_WIFI_SCANNING_ACTIVE "1"
#define BLE_WIFI_SCANNING_DEACTIVE "0"
#define BLE_WIFI_NOT_CONNECTED "0"
#define BLE_WIFI_CONNECTING "1"
#define BLE_WIFI_CONNECTED "2"

#define BLE_DEVICE_NAME "Google Home Node - RSv1"
#define BLE_SYSTEM_ID "Samira"
#define BLE_MANUFACTURER "Espressif Systems"
#define BLE_MODEL_NUMBER "Home Sensor"
#define BLE_FIRMWARE_REVISION "v1"
#define BLE_HARDWARE_REVISION "v1"
#define BLE_SOFTWARE_REVISION "v1"

enum ble_events
{
    BLE_CONNECTED,
    BLE_DISCONNECT,
    WIFI_START_SCAN,
    WIFI_CONNECTION_CHANGED
};

void BLEinit(std::string deviceName, bool *hasEvent);
void BLEsetupAd();
void BLEstartAd();
void BLEstopAd();
void BLEsetSSIDs(std::string SSIDs);
void BLEwirelessConnectionChanged(std::string status);
std::string BLEgetSSIDs();
std::string BLEgetSSID();
std::string BLEgetPassword();
ble_events BLEreadEvent();
void addCharacteristic(BLEService *pService, BLEUUID uuid, uint32_t properties, std::string value);
void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value);
std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid);