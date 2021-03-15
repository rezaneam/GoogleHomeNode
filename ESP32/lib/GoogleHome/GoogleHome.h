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
    bool NotifyTemperatureSummary(int average, int min, int max, Languages language = Languages::English);
    bool NotifyHumidity(int humidity, Languages language = Languages::English);
    bool NotifyHumiditySummary(int average, int min, int max, Languages language = Languages::English);
    bool NotifyPressure(float pressure, Languages language = Languages::English);

    bool Connected;

private:
#define WELCOME_MSG_EN "Hello!. Google node is initialized and ready to use."

    const char *Temperature_EN[3] PROGMEM =
        {
            "It is almost %d degrees right now.",
            "Right now, it is roughly %d degrees.",
            "Currently, it is about %d degrees."};

    const char *Temperature_DE[3] PROGMEM = {
        "Im Moment sind es ungefähr %d Grad.",
        "Es ist jetzt fast %d Grad.",
        "Derzeit ist es etwa %d Grad."};

    const char *HUMIDITY_EN[3] PROGMEM =
        {
            "The current humidity level is %d percent.",
            "The humidity level is approximately %d percent.",
            "The level of humidity is around %d percent."};

    const char *HUMIDITY_DE[3] PROGMEM =
        {
            "Die aktuelle Luftfeuchtigkeit beträgt %d Prozent. ",
            "Die Luftfeuchtigkeit beträgt ca. %d Prozent.",
            "Die Luftfeuchtigkeit beträgt ungefähr %d Prozent."};

    const char *Temperature_Summary_EN[1] PROGMEM =
        {
            "Today, the average temperature was %d and fluctuated between %d and %d."};

    const char *Temperature_Summary_DE[1] PROGMEM =
        {
            "Heute lag die Durchschnittstemperatur bei %d und schwankte zwischen %d und %d."};

    const char *Humidity_Summary_EN[1] PROGMEM =
        {
            "Today, the average humidity level was %d percent and fluctuated between %d and %d."};

    const char *Humidity_Summary_DE[1] PROGMEM =
        {
            "Die durchschnittliche Luftfeuchtigkeit lag heute bei %d Prozent und schwankte zwischen %d und %d."};

    std::string deviceName;
    const char *resolveLanguage(Languages lang);
    bool notify(std::string deviceName, std::string message, Languages langauge);
    void (*queueEvent)(CustomEvents);
    GoogleHomeNotifier googleHomeNotifier;
    bool isVerbose;
};
