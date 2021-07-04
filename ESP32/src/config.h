#pragma once

#define SENSOR_SDA_PIN 21
#define SENSOR_SCL_PIN 22
#define OLED_SDA_PIN 21// 5 // 21
#define OLED_SCL_PIN 22 // 4 // 22
#define SDA_Frequency 400000
#define OLED_Address 0x3c

#define BLE_ADVERTISE_TIMEOUT_S 60  // BLE advertising timeout
#define BLE_ADVERTISE_ENABLE_PIN 25 //02 // 33 // 15
#define BLE_ADVERTISE_LED_PIN 14
#define SENSOR_READ_INTERVAL 60

#define UPDATE_BSEC_STATE_INTERVAL 6         // interval of Update/Store status of BME680 into Flash in hours
#define RETRY_UPDATE_BSEC_STATE_INTERVAL 0.5 // interval of retrying Update/Store status of BME680 into Flash in hours

#define START_OF_DAY_HOUR 06 // Start of each day for daliy report (Hour number)
#define START_OF_DAY_MIN 00  // Start of each day for daliy report (Minute number)

#define EEPROM_SIZE 512
#define SERIAL_BAUDRATE 115200

#define VERBOSE true