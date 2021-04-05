#pragma once
#include <WiFi.h>
#include <Arduino.h>
#include <enums.h>
#include <ESP32Ping.h>

#define CONFIG_NIMBLE_CPP_ENABLE_RETURN_CODE_TEXT

class Wireless
{
public:
    void Initialize(void (*event_queue_method)(CustomEvents), char *hostname = "Google-Home-Node", bool verbose = false);
    bool ScanNodes();
    bool TryConnect(std::string SSID, std::string Password);
    void Disconnect();
    bool IsOnline(uint8_t iterations = 1);
    bool IsConnected(uint8_t iterations = 1);
    std::string SSIDs;

private:
    void (*queueEventWiFi)(CustomEvents);
    String translateEncryptionType(wifi_auth_mode_t encryptionType);
    bool isVerbose;
    bool isConnected;
    IPAddress gatewayIP;
    IPAddress localIP;
};