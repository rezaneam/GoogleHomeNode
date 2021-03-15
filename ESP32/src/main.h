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
#include <config.h>

#define BLE_ADVERTISE_TIMEOUT_MS 1000 * BLE_ADVERTISE_TIMEOUT_S // BLE advertising timeout

EnvironmentSensor Sensor;
SSD1306Wire Oled(OLED_Address, OLED_SDA_PIN, OLED_SCL_PIN);

bool tryStartBLE = false;
bool readSenor = false;
bool secondFlag = false;
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
void RefreshOLED();
void getTimeString(char *buffer);

AzureIoTHub azureIoT;
Wireless wireless;
GoogleHome googleHome;
BLElite BluetoothLE;

hw_timer_t *sensorReadTimer = NULL;
portMUX_TYPE sensorReadtimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE externalPinmux = portMUX_INITIALIZER_UNLOCKED;

uint16_t ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
uint16_t timerCalls = 0;
uint8_t changeDisplayTimeout = 4;

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
    if (timerCalls % 1000 == 0)
        secondFlag = true;
    if (isBLEadvertising)
    {
        if (timerCalls % 1000 == 0)
            digitalWrite(BLE_ADVERTISE_LED_PIN, HIGH);
        else if (timerCalls % 50 == 0)
            digitalWrite(BLE_ADVERTISE_LED_PIN, LOW);
    }
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