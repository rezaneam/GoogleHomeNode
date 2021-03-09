#pragma once
#include <Arduino.h>
#include <enums.h>
#include <esp8266-google-home-notifier.h>

class GoogleHome
{
public:
    bool Initialize(void (*event_queue_method)(CustomEvents), bool verbose = false);
    bool TryConnect(std::string deviceName);
    bool NotifyTemperature(int temperature, Languages language = Languages::English);
    bool NotifyTemperatureSummary(int min, int max, Languages language = Languages::English);
    bool NotifyHumidity(int humidity, Languages language = Languages::English);
    bool NotifyPressure(float pressure, Languages language = Languages::English);

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

    const char *Temperature_Summary_EN[3] PROGMEM =
        {
            "The minimum and maximum temperatures of today are %d and %d degrees.",
            "Today, the lowest temperature was %d and the highest temperature was %d.",
            "Today the temperature fluctuated between %d and %d."};

    const char *Temperature_Summary_DE[3] PROGMEM =
        {
            "Die minimalen und maximalen Temperaturen von heute betragen %d und %d Grad.",
            "Heute war die niedrigste Temperatur %d und die höchste Temperatur %d.",
            "Heute schwankte die Temperatur zwischen %d und %d."};

    std::string deviceName;
    const char *resolveLanguage(Languages lang);
    bool notify(std::string deviceName, std::string message, Languages langauge);
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
};
