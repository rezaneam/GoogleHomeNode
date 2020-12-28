#include <OLEDExtended.h>

void OLEDDisplayExtended::clearArea(int16_t x, int16_t y, int16_t width, int16_t height)
{
    OLEDDISPLAY_COLOR sourceColor = this->color;
    this->color = BLACK;
    this->fillRect(x, y, width, height);
    this->color = sourceColor;
}

void OLEDDisplayExtended::clearArea(int16_t area[4])
{
    this->clearArea(area[0], area[1], area[2], area[3]);
}

void OLEDDisplayExtended::drawIcon(OLEDDISPLAY_ICONS icon)
{
    switch (icon)
    {
    case OLEDDISPLAY_ICONS::BLE_ADVERTISING_ICON:
        this->drawXbm(BLE_advertising_icon_pos[0], BLE_advertising_icon_pos[1], BLE_advertising_icon_pos[2], BLE_advertising_icon_pos[3], BLE_advertising_icon_img);
        break;
    case OLEDDISPLAY_ICONS::BLE_CONNECTED_ICON:
        this->drawXbm(BLE_connected_icon_pos[0], BLE_connected_icon_pos[1], BLE_connected_icon_pos[2], BLE_connected_icon_pos[3], BLE_connected_icon_img);
        break;
    case OLEDDISPLAY_ICONS::WIFI_ICON:
        this->drawXbm(WiFi_connected_icon_pos[0], WiFi_connected_icon_pos[1], WiFi_connected_icon_pos[2], WiFi_connected_icon_pos[3], WiFi_connected_icon_img);
        break;
    default:
        break;
    }
}

void OLEDDisplayExtended::clearIcon(OLEDDISPLAY_ICONS icon)
{
    switch (icon)
    {
    case BLE_ADVERTISING_ICON:
        this->clearArea(BLE_advertising_icon_pos[0], BLE_advertising_icon_pos[1], BLE_advertising_icon_pos[2], BLE_advertising_icon_pos[3]);
        break;
    case BLE_CONNECTED_ICON:
        this->clearArea(BLE_connected_icon_pos[0], BLE_connected_icon_pos[1], BLE_connected_icon_pos[2], BLE_connected_icon_pos[3]);
        break;
    case WIFI_ICON:
        this->clearArea(WiFi_connected_icon_pos[0], WiFi_connected_icon_pos[1], WiFi_connected_icon_pos[2], WiFi_connected_icon_pos[3]);
        break;
    default:
        break;
    }
}

void OLEDDisplayExtended::BLEadvertising(bool isAdvertising)
{
    this->clearIcon(BLE_ADVERTISING_ICON);
    if (isAdvertising)
        this->drawIcon(BLE_ADVERTISING_ICON);
    this->display();
}

void OLEDDisplayExtended::BLEconnected(bool isConnected)
{
    this->clearIcon(BLE_CONNECTED_ICON);
    if (isConnected)
        this->drawIcon(BLE_CONNECTED_ICON);
    else
        this->drawIcon(BLE_ADVERTISING_ICON);
    this->display();
}

void OLEDDisplayExtended::WiFiconnected(bool isConnected, std::string ssid)
{
    this->clearIcon(WIFI_ICON);
    if (isConnected)
        this->drawIcon(WIFI_ICON);

    this->display();
}

void OLEDDisplayExtended::RefressSensorArea(float temperature, float humidity, float pressure)
{
    this->clearArea(Sensor_Area[0], Sensor_Area[1], Sensor_Area[2], Sensor_Area[3]);
    this->drawString(Sensor_Area[0], Sensor_Area[1], "T: " + String(temperature) + " C");
    this->drawString(Sensor_Area[0], Sensor_Area[1] + 16, "H: " + String(humidity) + " %");
    this->drawString(Sensor_Area[0], Sensor_Area[1] + 32, "P: " + String(pressure / 101325) + " atm");
    this->display();
}