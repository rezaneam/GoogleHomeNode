#pragma once
#include <Arduino.h>
#include <enums.h>
#include <GoogleHomeMsg.h>
#include <esp8266-google-home-notifier.h>

class GoogleHome
{
public:
    bool Initialize(void (*event_queue_method)(CustomEvents), bool verbose = false);
    bool TryConnect(std::string deviceName);
    bool NotifyTemperature(int temperature);
    bool NotifyHumidity(int humidity);
    bool NotifyPressure(int pressure);

    bool Connected;

private:
    std::string deviceName;
    bool Notify(std::string deviceName, std::string message);
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
};
