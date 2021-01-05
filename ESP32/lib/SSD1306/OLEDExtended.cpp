#include <OLEDExtended.h>

void OLEDDisplayExtended::Initialize(bool flip)
{
    init();
    if (flip)
        flipScreenVertically();
    clear();
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(ArialMT_Plain_24);
    this->drawString(sensor_icon_pos[0], sensor_icon_pos[1], "Initializing...");

    setFont(ArialMT_Plain_16);
    this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], "Please wait.");
    this->display();
}

void OLEDDisplayExtended::ShowRestMessage(String message)
{
    clear();
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(ArialMT_Plain_24);
    this->drawString(sensor_icon_pos[0], sensor_icon_pos[1], message);

    setFont(ArialMT_Plain_16);
    this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], "Please wait.");
    this->display();
}

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
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], BLE_advertising_icon_img);
        break;
    case OLEDDISPLAY_ICONS::BLE_CONNECTED_ICON:
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], BLE_connected_icon_img);
        break;
    case OLEDDISPLAY_ICONS::WIFI_ICON:
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], WiFi_connected_icon_img);
        break;
    case OLEDDISPLAY_ICONS::GOOGLE_HOME:
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], Google_Home_icon_img);
        break;
    case OLEDDISPLAY_ICONS::CLOUD:
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], Cloud_icon_img);
        break;
    default:
        break;
    }
    offset += 16;
}

void OLEDDisplayExtended::RefressSensorArea(float temperature, float humidity, float pressure)
{
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(ArialMT_Plain_16);
    if (!isSensorIconDrawn)
        drawSensorIcon();
    this->clearArea(Sensor_Text_Area[0], Sensor_Text_Area[1], Sensor_Text_Area[2], Sensor_Text_Area[3]);
    this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(temperature, 1));
    this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(humidity, 1));
    this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(pressure / 101325));
    this->display();
}

void OLEDDisplayExtended::drawSensorIcon()
{
    isSensorIconDrawn = true;
    this->clearArea(Sensor_Icon_Area[0], Sensor_Icon_Area[1], Sensor_Icon_Area[2], Sensor_Icon_Area[3]);
    this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Temperature_Sensor_icon_img);
    this->drawXbm(sensor_icon_pos[0] + 42, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Humidity_Sensor_icon_img);
    this->drawXbm(sensor_icon_pos[0] + 84, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Barometer_Sensor_icon_img);
}

void OLEDDisplayExtended::ReferessStatusArea(bool isBLEadvertising, bool isBLEconnected, bool isHomeConencted, bool isWiFiconnected, std::string ssid)
{
    this->clearArea(Status_Area[0], Status_Area[1], Status_Area[2], Status_Area[3]);
    offset = 0;
    if (isBLEadvertising)
        this->drawIcon(OLEDDISPLAY_ICONS::BLE_ADVERTISING_ICON);
    if (isBLEconnected)
        this->drawIcon(OLEDDISPLAY_ICONS::BLE_CONNECTED_ICON);
    if (isHomeConencted)
        this->drawIcon(OLEDDISPLAY_ICONS::GOOGLE_HOME);
    // if (isCloudConencted)
    //     this->drawIcon(OLEDDISPLAY_ICONS::CLOUD);
    if (isWiFiconnected)
    {
        this->drawIcon(OLEDDISPLAY_ICONS::WIFI_ICON);
        setTextAlignment(TEXT_ALIGN_LEFT);
        setFont(Roboto_10);
        this->drawString(Status_Area[0] + offset, Status_Area[1], ssid.c_str());
    }
    this->display();
}