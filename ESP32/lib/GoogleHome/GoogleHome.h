#pragma once
#include <Arduino.h>
#include <enums.h>
#include <esp8266-google-home-notifier.h>

class GoogleHome
{
public:
    bool Initialize(void (*event_queue_method)(CustomEvents), bool verbose = false);
    bool TryConnect(std::string deviceName);
    bool Notify(std::string deviceName, std::string message);
    bool Connected;

private:
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
#define Notifier_WELCOME_MSG "Hello!. Google node is initialized and ready to use."
};

