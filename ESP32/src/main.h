#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <SSD1306.h>
#include <Wireless.h>
#include <BME280.h>
#include <vector>
#include <security.h>
#include <Azure.h>
#include <GoogleHome.h>
//#include <blesmall.h>
#include <BLElite.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_ENABLE_PIN 15

#define SENSOR_READ_INTERVAL 60
#define EEPROM_SIZE 128
#define SERIAL_BAUDRATE 115200

#define VERBOSE true

BME280 Sensor = BME280();
SSD1306Wire Oled(OLED_Address, SDA_PIN, SCL_PIN);
bool tryStartBLE = false;
bool readSenor = false;
bool fireIoT = false;
bool isHomeConnected = false;
bool isBLEadvertising = false;
bool isBLEconnected = false;
bool isWiFiconnected = false;
bool isCloudconnected = false;
std::vector<CustomEvents> eventQueue;

std::string ssid;

hw_timer_t *sensorReadTimer = NULL;
portMUX_TYPE sensorReadtimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE externalPinmux = portMUX_INITIALIZER_UNLOCKED;

void EnqueueEvent(CustomEvents newEvent);
CustomEvents DequeueEvent();
void ConfigureTime();
void UpdateStatus(bool BLE, bool OLED);

AzureIoTHub azureIoT;
Wireless wireless;
GoogleHome googleHome;
BLElite BluetoothLE;