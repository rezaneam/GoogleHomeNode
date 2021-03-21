#pragma once

#define SENSOR_SDA_PIN 5
#define SENSOR_SCL_PIN 4
#define OLED_SDA_PIN 21 // 5 // 21
#define OLED_SCL_PIN 22 // 4 // 22
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_TIMEOUT_S 60  // BLE advertising timeout
#define BLE_ADVERTISE_ENABLE_PIN 15 //02 // 33 // 15
#define BLE_ADVERTISE_LED_PIN 12
#define SENSOR_READ_INTERVAL 60

#define EEPROM_SIZE 256
#define SERIAL_BAUDRATE 115200

#define VERBOSE false