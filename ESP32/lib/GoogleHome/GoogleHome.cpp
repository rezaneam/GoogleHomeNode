#include <GoogleHome.h>

bool GoogleHome::Initialize(void (*event_queue_method)(CustomEvents), bool verbose)
{
    isVerbose = verbose;
    queueEvent = event_queue_method;
}

bool GoogleHome::TryConnect(std::string deviceName)
{
    //const IPAddress ip = IPAddress(192, 168, 1, 108); //= "192.168.1.108";
    this->deviceName = deviceName;
    if (Connected)
        return true;

    if (isVerbose)
        printf("Connecting to a Google Home Speaker: %s \r\n", deviceName.c_str());
    if (googleHomeNotifier.device(deviceName.c_str(), "en") != true)
    {
        if (isVerbose)
            printf("Failed to connect to the Google Home Speaker. Error: %s\r\n", googleHomeNotifier.getLastError());
        return false;
    }
    else
    {
        Connected = true;
        if (isVerbose)
            printf("Found the Google Home Speaker. %s (%s:%d).\r\n",
                   deviceName.c_str(),
                   googleHomeNotifier.getIPAddress()
                       .toString()
                       .c_str(),
                   googleHomeNotifier.getPort());

        // char buffer[200];
        // sprintf(buffer, "Hey there. Sensor is now connected to %s!\r\n", deviceName.c_str());
        // if (!googleHomeNotifier.notify(buffer))
        // {
        //     if (isVerbose)
        //         printf("Google Home notify error: %s\r\n", googleHomeNotifier.getLastError());
        //     Connected = false;
        // }
    }

    if (Connected)
        queueEvent(CustomEvents::EVENT_GOOGLE_HOME_CONNECTED);

    return Connected;
}

const char *GoogleHome::resolveLanguage(Languages lang)
{
    switch (lang)
    {
    case Languages::English:
        return (const char *)"en";

    case Languages::Deutsch:
        return (const char *)"de";

    case Languages::Francais:
        return (const char *)"fr";
    default:
        return (const char *)"en";
    }
}
bool GoogleHome::notify(std::string deviceName, std::string message, Languages language)
{
    if (!TryConnect(deviceName))
        return false;
    googleHomeNotifier.setLanguage(resolveLanguage(language));
    if (!googleHomeNotifier.notify(message.c_str()))
    {
        if (isVerbose)
            printf("Google Home notify error: %s\r\n", googleHomeNotifier.getLastError());
        return false;
    }
    return true;
}

bool GoogleHome::NotifyTemperature(int temperature, Languages language)
{
    char buffer[200];
    byte index = rand() % 3;

    switch (language)
    {
    case Languages::English:
        sprintf(buffer, Temperature_EN[index], temperature);
        break;
    case Languages::Deutsch:
        sprintf(buffer, Temperature_DE[index], temperature);
        break;
    case Languages::Francais:
        sprintf(buffer, Temperature_FR[index], temperature);
        break;
    default:
        break;
    }
    return notify(this->deviceName, buffer, language);
}

bool GoogleHome::NotifyTemperatureSummary(int average, int min, int max, Languages language)
{
    char buffer[200];
    byte index = 0;

    switch (language)
    {
    case Languages::English:
        sprintf(buffer, Temperature_Summary_EN[index], average, min, max);
        break;
    case Languages::Deutsch:
        sprintf(buffer, Temperature_Summary_DE[index], average, min, max);
        break;
    case Languages::Francais:
        sprintf(buffer, Temperature_Summary_FR[index], average, min, max);
        break;
    default:
        break;
    }
    return notify(this->deviceName, buffer, language);
}

bool GoogleHome::NotifyHumidity(int humidity, Languages language)
{
    char buffer[200];
    byte index = rand() % 3;
    switch (language)
    {
    case Languages::English:
        sprintf(buffer, HUMIDITY_EN[index], humidity);
        break;
    case Languages::Deutsch:
        sprintf(buffer, HUMIDITY_DE[index], humidity);
        break;
    case Languages::Francais:
        sprintf(buffer, HUMIDITY_FR[index], humidity);
        break;
    default:
        break;
    }

    return notify(this->deviceName, buffer, language);
}

bool GoogleHome::NotifyHumiditySummary(int average, int min, int max, Languages language)
{
    char buffer[200];
    byte index = 0;

    switch (language)
    {
    case Languages::English:
        sprintf(buffer, Humidity_Summary_EN[index], average, min, max);
        break;
    case Languages::Deutsch:
        sprintf(buffer, Humidity_Summary_DE[index], average, min, max);
        break;
    case Languages::Francais:
        sprintf(buffer, Humidity_Summary_FR[index], average, min, max);
        break;
    default:
        break;
    }
    return notify(this->deviceName, buffer, language);
}

bool GoogleHome::NotifyPressure(float pressure, Languages language)
{
    char buffer[200];
    sprintf(buffer, "The current temperature level is %2.1f atm", pressure);
    return notify(this->deviceName, buffer, language);
}
