#include <Arduino.h>
#include <OLEDDisplay.h>

const int16_t BLE_advertising_icon_pos[] PROGMEM = {0, 0, 16, 16};
const unsigned char BLE_advertising_icon_img[] PROGMEM = {
    0x80, 0x01, 0x80, 0x03, 0x80, 0x07, 0x98, 0x0F,
    0xB8, 0x0D, 0xF0, 0x0D, 0xE0, 0x07, 0xC0, 0x03,
    0x80, 0x03, 0xE0, 0x07, 0xF0, 0x0D, 0xB8, 0x0D,
    0x98, 0x0F, 0x80, 0x07, 0x80, 0x03, 0x80, 0x01};

const int16_t BLE_connected_icon_pos[] PROGMEM = {0, 0, 16, 16};
const unsigned char BLE_connected_icon_img[] PROGMEM = {
    0x80, 0x01, 0x80, 0x03, 0x80, 0x07, 0x98, 0x0F,
    0xB8, 0x0D, 0xF0, 0x0D, 0xE0, 0x07, 0xCE, 0x73,
    0x8E, 0x73, 0xE0, 0x07, 0xF0, 0x0D, 0xB8, 0x0D,
    0x98, 0x0F, 0x80, 0x07, 0x80, 0x03, 0x80, 0x01};

const int16_t WiFi_connected_icon_pos[] PROGMEM = {16, 0, 16, 16};
const unsigned char WiFi_connected_icon_img[] PROGMEM = {
    0x80, 0x01, 0xF0, 0x0F, 0x7C, 0x3E, 0x0E, 0x70,
    0x82, 0x41, 0xF0, 0x0F, 0x38, 0x1C, 0x08, 0x10,
    0xC0, 0x03, 0xE0, 0x07, 0x20, 0x04, 0x00, 0x00,
    0x80, 0x01, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x01};

enum OLEDDISPLAY_ICONS
{
    BLE_ADVERTISING_ICON = 0,
    BLE_CONNECTED_ICON = 1,
    WIFI_ICON = 2
};

class OLEDDisplayExtended : public OLEDDisplay
{
public:
    // Clearing a rectangualr area of the screen
    void clearArea(int16_t x, int16_t y, int16_t width, int16_t height);

    // Clearing a rectangular area of the screen
    void clearArea(int16_t area[4]);

    // Drawing the Icons
    void drawIcon(OLEDDISPLAY_ICONS icon);

    // clearing the Icon area
    void clearIcon(OLEDDISPLAY_ICONS icon);

    // Show the BLE icon if advertising otherwise clear the icon area.
    void BLEadvertising(bool isAdvertising);

    // Show BLE icon connected if connected, otherwise show the bluetooth icon
    void BLEconnected(bool isConnected);

    // Show WiFi icon including the WiFi SSID if connected
    void WiFiconnected(bool isConnected, std::string ssid);
};
