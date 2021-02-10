#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <SSD1306.h>
#include <Wireless.h>
#include <EnvironmentSensor.h>
#include <vector>
#include <Azure.h>
#include <GoogleHome.h>
#include <BLElite.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_TIMEOUT_S 60                              // BLE advertising timeout
#define BLE_ADVERTISE_TIMEOUT_MS 1000 * BLE_ADVERTISE_TIMEOUT_S // BLE advertising timeout
#define BLE_ADVERTISE_ENABLE_PIN 15
#define SENSOR_READ_INTERVAL 60

#define EEPROM_SIZE 256
#define SERIAL_BAUDRATE 115200

#define VERBOSE true

EnvironmentSensor Sensor;
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

void EnqueueEvent(CustomEvents newEvent);
CustomEvents DequeueEvent();
void ConfigureTime();
void UpdateStatus(bool BLE, bool OLED);

AzureIoTHub azureIoT;
Wireless wireless;
GoogleHome googleHome;
BLElite BluetoothLE;

hw_timer_t *sensorReadTimer = NULL;
portMUX_TYPE sensorReadtimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE externalPinmux = portMUX_INITIALIZER_UNLOCKED;

uint16_t ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
uint16_t timerCalls = 0;
void IRAM_ATTR handleExternalInterrupt()
{
    portENTER_CRITICAL_ISR(&externalPinmux);
    tryStartBLE = true;
    portEXIT_CRITICAL_ISR(&externalPinmux);
}

void IRAM_ATTR onReadSensor()
{
    portENTER_CRITICAL_ISR(&sensorReadtimerMux);
    timerCalls++;
    if (timerCalls % 50 == 0)
        fireIoT = true;
    if (timerCalls == SENSOR_READ_INTERVAL * 1000)
    { // Read sensor and update screen
        readSenor = true;
        timerCalls = 0;
    }

    if (isBLEadvertising && !isBLEconnected)
        ble_advertize_timeOut--;

    portEXIT_CRITICAL_ISR(&sensorReadtimerMux);
}

void initializeTimer()
{
    sensorReadTimer = timerBegin(1, 80, true);
    timerAttachInterrupt(sensorReadTimer, &onReadSensor, true);
    timerAlarmWrite(sensorReadTimer, 1000, true);
    timerAlarmEnable(sensorReadTimer);
}