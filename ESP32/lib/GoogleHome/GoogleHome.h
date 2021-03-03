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
    bool NotifyPressure(float pressure);

    bool Connected;

private:
#define WELCOME_MSG_EN "Hello!. Google node is initialized and ready to use."

    const char *Temperature_EN[3] PROGMEM =
        {
            "The current temperature is %d",
            "It is %d degrees now",
            "Currently, it is %d degrees"};

    const char *HUMIDITY_EN[3] PROGMEM =
        {
            "The current temperature level is %d ",
            "It is %d %% humide now",
            "Currently, it is %d %% humide"};

    std::string deviceName;
    bool Notify(std::string deviceName, std::string message);
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
};
