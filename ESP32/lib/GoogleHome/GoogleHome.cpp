#include <GoogleHome.h>

bool GoogleHome::Initialize(void (*event_queue_method)(CustomEvents), bool verbose)
{
    isVerbose = verbose;
    queueEvent = event_queue_method;
}

bool GoogleHome::TryConnect(std::string deviceName)
{
    this->deviceName = deviceName;
    if (Connected)
        return true;

    if (isVerbose)
        printf("connecting to Google Home: %s \r\n", deviceName.c_str());
    if (googleHomeNotifier.device(deviceName.c_str(), "en") != true)
    {
        if (isVerbose)
            printf("Google Home connection error: %s\r\n", googleHomeNotifier.getLastError());
        return false;
    }
    else
    {
        Connected = true;
        if (isVerbose)
            printf("Found Google Home(%s : %d).\r\n",
                   googleHomeNotifier.getIPAddress().toString().c_str(),
                   googleHomeNotifier.getPort());
        // if (googleHomeNotifier.notify(Notifier_WELCOME_MSG) != true)
        // {
        //     if (isVerbose)
        //   printf("Google Home notify error: %s\r\n", googleHomeNotifier.getLastError());
        //   Connected = false;
        // }
    }
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
