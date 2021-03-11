#include <Arduino.h>
#include <OLEDDisplay.h>
#include <enums.h>

const unsigned char Arrow_Down_icon_img[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x7E, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char Arrow_Up_icon_img[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char BLE_advertising_icon_img[] PROGMEM = {
    0x80, 0x01, 0x80, 0x03, 0x80, 0x07, 0x98, 0x0F,
    0xB8, 0x0D, 0xF0, 0x0D, 0xE0, 0x07, 0xC0, 0x03,
    0x80, 0x03, 0xE0, 0x07, 0xF0, 0x0D, 0xB8, 0x0D,
    0x98, 0x0F, 0x80, 0x07, 0x80, 0x03, 0x80, 0x01};

const unsigned char Cloud_icon_img[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x08,
    0x58, 0x10, 0x24, 0x10, 0x02, 0x20, 0x02, 0x60,
    0x01, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x02, 0xC0,
    0xFC, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char BLE_connected_icon_img[] PROGMEM = {
    0x80, 0x01, 0x80, 0x03, 0x80, 0x07, 0x98, 0x0F,
    0xB8, 0x0D, 0xF0, 0x0D, 0xE0, 0x07, 0xCE, 0x73,
    0x8E, 0x73, 0xE0, 0x07, 0xF0, 0x0D, 0xB8, 0x0D,
    0x98, 0x0F, 0x80, 0x07, 0x80, 0x03, 0x80, 0x01};

const unsigned char WiFi_connected_icon_img[] PROGMEM = {
    0x80, 0x01, 0xF0, 0x0F, 0x7C, 0x3E, 0x0E, 0x70,
    0x82, 0x41, 0xF0, 0x0F, 0x38, 0x1C, 0x08, 0x10,
    0xC0, 0x03, 0xE0, 0x07, 0x20, 0x04, 0x00, 0x00,
    0x80, 0x01, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x01};

const unsigned char Google_Home_icon_img[] PROGMEM = {
    0x00, 0x00, 0x80, 0x01, 0x60, 0x06, 0x30, 0x0C,
    0x18, 0x18, 0x0C, 0x30, 0x06, 0x60, 0x08, 0x10,
    0x08, 0x10, 0x08, 0x10, 0xC8, 0x13, 0x48, 0x12,
    0x48, 0x12, 0x78, 0x1E, 0x00, 0x00, 0x00, 0x00};

const unsigned char Barometer_Sensor_icon_img[] PROGMEM = {
    0x00, 0xC0, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0x00,
    0x00, 0xFE, 0x7F, 0x00, 0x80, 0x1F, 0xF8, 0x01,
    0xC0, 0x03, 0xC0, 0x03, 0xE0, 0x01, 0x80, 0x07,
    0x70, 0x00, 0x00, 0x0E, 0x38, 0x00, 0x00, 0x0C,
    0x38, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x20, 0x38,
    0x0C, 0x00, 0x70, 0x30, 0x0C, 0x00, 0x38, 0x30,
    0x0E, 0x00, 0x1C, 0x70, 0x06, 0x00, 0x0E, 0x70,
    0x07, 0xC0, 0x07, 0xE0, 0x07, 0xC0, 0x03, 0xE0,
    0x07, 0xC0, 0x03, 0xE0, 0x07, 0xE0, 0x03, 0xE0,
    0x06, 0x7E, 0x00, 0x70, 0x0E, 0x3F, 0x00, 0x70,
    0x0C, 0x30, 0x00, 0x30, 0x0C, 0x30, 0x00, 0x30,
    0x1C, 0x30, 0x00, 0x38, 0x38, 0x10, 0x00, 0x1C,
    0x30, 0x00, 0x00, 0x0C, 0x70, 0x00, 0x00, 0x0E,
    0xE0, 0x01, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xDF, 0x17, 0x01, 0x00, 0x11, 0xB1, 0x01,
    0x00, 0x1F, 0x51, 0x01, 0x00, 0x11, 0x11, 0x01};

const unsigned char Humidity_Sensor_icon_img[] PROGMEM = {
    0x00, 0x80, 0x01, 0x00, 0x00, 0xC0, 0x03, 0x00,
    0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00,
    0x00, 0x30, 0x0C, 0x00, 0x00, 0x10, 0x08, 0x00,
    0x00, 0x18, 0x18, 0x00, 0x00, 0x0C, 0x30, 0x00,
    0x00, 0x04, 0x20, 0x00, 0x00, 0x06, 0x60, 0x00,
    0x00, 0x02, 0x40, 0x00, 0x00, 0x03, 0xC0, 0x00,
    0x00, 0x01, 0x80, 0x00, 0x80, 0x01, 0x80, 0x01,
    0x80, 0x60, 0x00, 0x01, 0xC0, 0x90, 0x08, 0x03,
    0xC0, 0x90, 0x04, 0x03, 0x40, 0x90, 0x02, 0x02,
    0x60, 0x60, 0x01, 0x06, 0x60, 0x80, 0x06, 0x06,
    0x60, 0x40, 0x09, 0x06, 0x60, 0x20, 0x09, 0x06,
    0x60, 0x10, 0x09, 0x02, 0x40, 0x00, 0x06, 0x02,
    0xC0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01,
    0x80, 0x01, 0x80, 0x01, 0x00, 0x03, 0xC0, 0x00,
    0x00, 0x0E, 0x70, 0x00, 0x00, 0x3C, 0x3C, 0x00,
    0x00, 0xF0, 0x0F, 0x00, 0x00, 0xC0, 0x03, 0x00};

const unsigned char Temperature_Sensor_icon_img[] PROGMEM = {
    0x00, 0xE0, 0x00, 0x00, 0x00, 0xB0, 0x01, 0x00,
    0x00, 0x18, 0x03, 0x00, 0x00, 0x08, 0x02, 0x00,
    0x00, 0x0C, 0x02, 0x00, 0x00, 0x0C, 0xF2, 0x00,
    0x00, 0x0C, 0x02, 0x00, 0x00, 0x0C, 0x02, 0x00,
    0x00, 0x0C, 0x02, 0x00, 0x00, 0x0C, 0x72, 0x00,
    0x00, 0x6C, 0x02, 0x00, 0x00, 0x6C, 0x02, 0x00,
    0x00, 0x6C, 0x02, 0x00, 0x00, 0x6C, 0xF2, 0x00,
    0x00, 0x6C, 0x02, 0x00, 0x00, 0x6C, 0x02, 0x00,
    0x00, 0x6C, 0x02, 0x00, 0x00, 0x6C, 0x02, 0x00,
    0x00, 0x6C, 0x02, 0x00, 0x00, 0x6C, 0x86, 0x00,
    0x00, 0x66, 0x44, 0x01, 0x00, 0xF2, 0x88, 0x00,
    0x00, 0xF9, 0x19, 0x1C, 0x00, 0xFD, 0x13, 0x22,
    0x00, 0xFD, 0x13, 0x01, 0x00, 0xFD, 0x13, 0x01,
    0x00, 0xF9, 0x19, 0x01, 0x00, 0xF3, 0x08, 0x22,
    0x00, 0x66, 0x0C, 0x1C, 0x00, 0x0C, 0x06, 0x00,
    0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char Air_Quality_Sensor_icon_img[] PROGMEM = {
    0x00, 0x80, 0x0F, 0x00, 0x00, 0xC0, 0x38, 0x00,
    0x00, 0x60, 0x70, 0x00, 0x00, 0x70, 0xE0, 0x00,
    0x00, 0x30, 0xC0, 0x00, 0x00, 0x18, 0x80, 0x01,
    0x00, 0x0C, 0x00, 0x03, 0x00, 0x0C, 0x00, 0x03,
    0x00, 0x18, 0x80, 0x01, 0x40, 0x30, 0xC0, 0x00,
    0xE0, 0x70, 0xE0, 0x00, 0xB0, 0xE1, 0x70, 0x00,
    0x18, 0xC3, 0x39, 0x00, 0x0C, 0x06, 0xCF, 0x00,
    0x0C, 0x06, 0xE6, 0x01, 0x18, 0x63, 0x30, 0x03,
    0xB0, 0xF1, 0x18, 0x06, 0xE0, 0x98, 0x0D, 0x0C,
    0x40, 0x98, 0x05, 0x18, 0x00, 0xF0, 0x04, 0x18,
    0x00, 0x60, 0x0C, 0x0C, 0x00, 0x00, 0x18, 0x06,
    0x00, 0x80, 0x31, 0x03, 0x00, 0xC0, 0xE3, 0x00,
    0x00, 0x60, 0xC6, 0x00, 0x00, 0x30, 0x0C, 0x00,
    0x00, 0x60, 0x06, 0x00, 0x00, 0xC2, 0x03, 0x00,
    0x00, 0x87, 0x01, 0x00, 0x80, 0x0D, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00};

const int16_t status_icon_pos[] PROGMEM = {0, 0, 16, 16};
const int16_t SSID_Area[] PROGMEM = {32, 0, 96, 16};
// const int16_t Sensor_Area[] PROGMEM = {0, 16, 128, 48};
const int16_t sensor_icon_pos[] PROGMEM = {0, 16, 32, 32};
const int16_t arrow_icon_pos[] PROGMEM = {0, 0, 8, 16};
const int16_t Sensor_Icon_Area[] PROGMEM = {0, 16, 128, 32};
const int16_t Sensor_Text_Area[] PROGMEM = {0, 48, 128, 16};
const int16_t Sensor_Area[] PROGMEM = {0, 16, 128, 48};
const int16_t Status_Area[] PROGMEM = {0, 0, 128, 16};

enum OLEDDISPLAY_ICONS
{
    BLE_ADVERTISING_ICON = 0,
    BLE_CONNECTED_ICON = 1,
    WIFI_ICON = 2,
    GOOGLE_HOME = 3,
    CLOUD = 4,
    SSID_PLACEHOLDER = 5
};

class OLEDDisplayExtended : public OLEDDisplay
{
private:
    int16_t offset = 0;

    // Clearing a rectangualr area of the screen
    void clearArea(int16_t x, int16_t y, int16_t width, int16_t height);

    // Clearing a rectangular area of the screen
    void clearArea(int16_t area[4]);

    // Drawing the Icons
    void drawIcon(OLEDDISPLAY_ICONS icon);

    void drawSensorIcon(bool isBME280, bool isBME680);

    const char *resolveWeekDay(int day);
    const char *resolveMonth(int month);

public:
    DisplayStatus CurrentShow;

    void Initialize(bool flip = false);

    void RefressSensorArea(float temperature, float humidity, float pressure, float air_quality = -1);
    void ShowMixMax(float current, float min, float max, DisplayStatus status);

    void ReferessStatusArea(bool isBLEadvertising, bool isBLEconnected, bool isHomeConencted, bool isWiFiconnected, std::string ssid, bool isCloudConencted);

    void ShowRestMessage(String message);

    void ShowDateTime(tm *ltm);
};
