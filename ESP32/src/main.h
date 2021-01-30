#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <SSD1306.h>
#include <Wireless.h>
#include <BME280.h>
#include <esp8266-google-home-notifier.h>
#include <security.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_ENABLE_PIN 15

#define SENSOR_READ_INTERVAL 60
#define EEPROM_SIZE 128
#define SERIAL_BAUDRATE 115200

#define VERBOSE true

#define Notifier_WELCOME_MSG "Hello!. Google node is initialized and ready to use."

GoogleHomeNotifier googleHomeNotifier;

BME280 Sensor = BME280();
SSD1306Wire Oled(OLED_Address, SDA_PIN, SCL_PIN);
bool readSenor = false;
bool isHomeConnected = false;
bool isBLEadvertising = false;
bool isBLEconnected = false;
bool isWiFiconnected = false;
bool isCloudconnected = false;
CustomEvents activeEvent = CustomEvents::EVENT_NONE;

std::string ssid;
char messageBuffer[64];

hw_timer_t *sensorReadTimer = NULL;
portMUX_TYPE sensorReadtimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE externalPinmux = portMUX_INITIALIZER_UNLOCKED;

bool NotifierTryConnect(std::string deviceName);
bool NotifierNotify(std::string deviceName, std::string message);

bool InitIoT();