#include <OLEDExtended.h>

void OLEDDisplayExtended::Initialize(bool flip)
{
    init();
    if (flip)
        flipScreenVertically();
    clear();
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(Roboto_Condensed_24);
    this->drawString(sensor_icon_pos[0], sensor_icon_pos[1], "Initializing...");

    setFont(Roboto_Condensed_16);
    this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], "Please wait.");
    this->display();
}

void OLEDDisplayExtended::ShowMessage(String title, String subtitle, String detail)
{
    clear();
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(Roboto_Condensed_20);
    this->drawString(0, 16, title);

    setFont(Roboto_Condensed_16);
    this->drawString(0, 36, subtitle);

    setFont(Roboto_Condensed_12);
    this->drawString(0, 52, detail);
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
    case OLEDDISPLAY_ICONS::NOTIFYING:
        this->drawXbm(status_icon_pos[0] + offset, status_icon_pos[1], status_icon_pos[2], status_icon_pos[3], Notify_icon_img);
        break;
    default:
        break;
    }
    offset += 16;
}

void OLEDDisplayExtended::ShowDateTime(tm *time)
{
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(Roboto_Condensed_18);
    char buffer[200];

    this->clearArea(Sensor_Area[0], Sensor_Area[1], Sensor_Area[2], Sensor_Area[3]);
    sprintf(buffer, "%s %d %s %d\r\n", resolveWeekDay(time->tm_wday), time->tm_mday, resolveMonth(time->tm_mon), 1900 + time->tm_year);
    this->drawString(0, 16, buffer);
    sprintf(buffer, "%d:%d\r\n", time->tm_hour, time->tm_min);
    setFont(Roboto_Condensed_24);
    this->drawString(36, 36, buffer);
    this->display();
    CurrentShow = DisplayStatus::Time;
}

const char *OLEDDisplayExtended::resolveMonth(int month)
{
    switch (month)
    {
    case 0:
        return (const char *)"Jan";
    case 1:
        return (const char *)"Feb";
    case 2:
        return (const char *)"Mar";
    case 3:
        return (const char *)"Apr";
    case 4:
        return (const char *)"May";
    case 5:
        return (const char *)"Jun";
    case 6:
        return (const char *)"Jul";
    case 7:
        return (const char *)"Aug";
    case 8:
        return (const char *)"Sep";
    case 9:
        return (const char *)"Oct";
    case 10:
        return (const char *)"Nov";
    case 11:
        return (const char *)"Dec";
    default:
        return (const char *)"Err";
    }
}
const char *OLEDDisplayExtended::resolveWeekDay(int day)
{
    switch (day)
    {
    case 0:
        return (const char *)"Sun";
    case 1:
        return (const char *)"Mon";
    case 2:
        return (const char *)"Tue";
    case 3:
        return (const char *)"Wed";
    case 4:
        return (const char *)"Thr";
    case 5:
        return (const char *)"Fri";
    case 6:
        return (const char *)"Sat";
    default:
        return (const char *)"Err";
    }
}

void OLEDDisplayExtended::ShowMSummary(float average, float min, float max, DisplayStatus sensor)
{
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(Roboto_Condensed_16);
    this->clearArea(Sensor_Area[0], Sensor_Area[1], Sensor_Area[2], Sensor_Area[3]);
    this->drawLine(4, 48, 24, 48);
    switch (sensor)
    {
    case DisplayStatus::TemperatureSensor:
        this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Temperature_Sensor_icon_img);
        this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(average, 1));
        this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(min, 1));
        this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(max, 1));
        break;
    case DisplayStatus::HumiditySensor:
        this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Humidity_Sensor_icon_img);
        this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(average, 1));
        this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(min, 1));
        this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(max, 1));
        break;
    case DisplayStatus::PressureSensor:
        this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Barometer_Sensor_icon_img);
        this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(average / 101325, 3));
        this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(min / 101325, 3));
        this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(max / 101325, 3));

        break;
    case DisplayStatus::AirQualitySensor:
        this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Air_Quality_Sensor_icon_img);
        this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(average, 1));
        this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(min, 0));
        this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(max, 0));
        break;
    default:
        break;
    }
    CurrentShow = sensor;
    setFont(Roboto_Condensed_20);
    this->drawString(42, 24, "Min");
    this->drawString(84, 24, "Max");
    this->display();
}
void OLEDDisplayExtended::RefressSensorArea(float temperature, float humidity, float pressure, float air_quality)
{
    bool isBME680 = air_quality != -1;
    bool isBME280 = !isBME680 && humidity != -1;
    setTextAlignment(TEXT_ALIGN_LEFT);
    setFont(ArialMT_Plain_16);
    this->clearArea(Sensor_Text_Area[0], Sensor_Text_Area[1], Sensor_Text_Area[2], Sensor_Text_Area[3]);

    drawSensorIcon(isBME280, isBME680);
    if (isBME280)
    {

        this->drawString(Sensor_Text_Area[0], Sensor_Text_Area[1], String(temperature, 1));
        this->drawString(Sensor_Text_Area[0] + 42, Sensor_Text_Area[1], String(humidity, 1));
        this->drawString(Sensor_Text_Area[0] + 84, Sensor_Text_Area[1], String(pressure / 101325));
    }
    else if (isBME680)
    {
        this->drawString(Sensor_Text_Area[0] + 4, Sensor_Text_Area[1], String(temperature, 0));
        this->drawString(Sensor_Text_Area[0] + 36, Sensor_Text_Area[1], String(humidity, 0));
        this->drawString(Sensor_Text_Area[0] + 64, Sensor_Text_Area[1], String(pressure / 101325));
        this->drawString(Sensor_Text_Area[0] + 100, Sensor_Text_Area[1], String(air_quality, 0));
    }
    else
    { // BMP280
        this->drawString(Sensor_Text_Area[0] + 16, Sensor_Text_Area[1], String(temperature, 1));
        this->drawString(Sensor_Text_Area[0] + 80, Sensor_Text_Area[1], String(pressure / 101325));
    }
    CurrentShow = DisplayStatus::AllSensors;
    this->display();
}

void OLEDDisplayExtended::drawSensorIcon(bool isBME280, bool isBME680)
{
    this->clearArea(Sensor_Icon_Area[0], Sensor_Icon_Area[1], Sensor_Icon_Area[2], Sensor_Icon_Area[3]);
    if (isBME680)
    {
        this->drawXbm(sensor_icon_pos[0] - 4, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Temperature_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 28, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Humidity_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 60, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Barometer_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 96, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Air_Quality_Sensor_icon_img);
    }
    else if (isBME280)
    {
        this->drawXbm(sensor_icon_pos[0], sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Temperature_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 42, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Humidity_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 84, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Barometer_Sensor_icon_img);
    }
    else
    { // BMP280
        this->drawXbm(sensor_icon_pos[0] + 16, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Temperature_Sensor_icon_img);
        this->drawXbm(sensor_icon_pos[0] + 80, sensor_icon_pos[1], sensor_icon_pos[2], sensor_icon_pos[3], Barometer_Sensor_icon_img);
    }
}

void OLEDDisplayExtended::ReferessStatusArea(
    bool isBLEadvertising, bool isBLEconnected,
    bool isHomeConencted,
    bool isWiFiconnected,
    std::string ssid,
    bool isCloudConencted,
    bool isNotifying)
{
    this->clearArea(Status_Area[0], Status_Area[1], Status_Area[2], Status_Area[3]);
    offset = 0;
    if (isBLEconnected)
        this->drawIcon(OLEDDISPLAY_ICONS::BLE_CONNECTED_ICON);
    else if (isBLEadvertising)
        this->drawIcon(OLEDDISPLAY_ICONS::BLE_ADVERTISING_ICON);
    if (isHomeConencted)
        this->drawIcon(OLEDDISPLAY_ICONS::GOOGLE_HOME);
    if (isCloudConencted)
        this->drawIcon(OLEDDISPLAY_ICONS::CLOUD);
    if (isNotifying)
        this->drawIcon(OLEDDISPLAY_ICONS::NOTIFYING);
    if (isWiFiconnected)
    {
        this->drawIcon(OLEDDISPLAY_ICONS::WIFI_ICON);
        setTextAlignment(TEXT_ALIGN_LEFT);
        setFont(Roboto_Condensed_12);
        this->drawString(Status_Area[0] + offset, Status_Area[1], ssid.c_str());
    }
    this->display();
}