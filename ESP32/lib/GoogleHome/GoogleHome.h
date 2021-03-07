#pragma once
#include <Arduino.h>
#include <enums.h>
#include <esp8266-google-home-notifier.h>

class GoogleHome
{
public:
    bool Initialize(void (*event_queue_method)(CustomEvents), bool verbose = false);
    bool TryConnect(std::string deviceName);
    bool NotifyTemperature(int temperature, Languages langauge = Languages::English);
    bool NotifyHumidity(int humidity, Languages langauge = Languages::English);
    bool NotifyPressure(float pressure, Languages langauge = Languages::English);

    bool Connected;

private:
#define WELCOME_MSG_EN "Hello!. Google node is initialized and ready to use."

    const char *Temperature_EN[3] PROGMEM =
        {
            "The current temperature is %d.",
            "It is %d degrees now.",
            "Currently, it is %d degrees."};

    const char *Temperature_DE[3] PROGMEM = {
        "Die aktuelle Temperatur beträgt %d Grad.",
        "Es ist jetzt %d Grad.",
        "Derzeit sind es %d Grad."};

    const char *HUMIDITY_EN[3] PROGMEM =
        {
            "The current humidity level is %d percent.",
            "It is %d %% humide now.",
            "Currently, it is %d %% humide."};

    const char *HUMIDITY_DE[3] PROGMEM =
        {
            "Die aktuelle Luftfeuchtigkeit beträgt %d Prozent. ",
            "Es ist jetzt %d Prozent Luftfeuchtigkeit.",
            "Derzeit ist es %d Prozent Luftfeuchtigkeit.."};

    std::string deviceName;
    const char *resolveLanguage(Languages lang);
    bool notify(std::string deviceName, std::string message, Languages langauge);
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
};
