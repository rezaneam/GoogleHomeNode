#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <SSD1306.h>
#include <Wireless.h>
#include <BME280.h>
#include <esp8266-google-home-notifier.h>

#define SDA_PIN 5
#define SCL_PIN 4
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_ENABLE_PIN 15

#define SENSOR_READ_INTERVAL 60
#define EEPROM_SIZE 128
#define SERIAL_BAUDRATE 115200

#define VERBOSE true