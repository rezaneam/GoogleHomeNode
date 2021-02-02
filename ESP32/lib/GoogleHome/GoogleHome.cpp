#include <GoogleHome.h>

bool GoogleHome::Initialize(void (*event_queue_method)(CustomEvents), bool verbose)
{
    isVerbose = verbose;
    queueEvent = event_queue_method;
}

bool GoogleHome::TryConnect(std::string deviceName)
{
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
            printf("Found Google Home(%s : %d).\r\n", googleHomeNotifier.getIPAddress().toString().c_str(), googleHomeNotifier.getPort());
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

bool GoogleHome::Notify(std::string deviceName, std::string message)
{
    if (!TryConnect(deviceName))
        return false;
    if (googleHomeNotifier.notify(message.c_str()) != true)
    {
        if (isVerbose)
            printf("Google Home notify error: %s\r\n", googleHomeNotifier.getLastError());
        return false;
    }
    return true;
}
