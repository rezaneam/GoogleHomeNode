#pragma once

#include <Arduino.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>
#include <enums.h>

#define AzureMethodName "Google Home"

class AzureIoTHub
{
public:
    bool Initialize(
        const char *securityKey,
        void (*event_queue_method)(const CustomEvents &),
        const char *username,
        const char *devicelocation,
        bool verbose = false);
    void HeartBeat();
    Languages GetLanguage();
    void Distroy();

private:
    IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
    int receiveContext = 0;
};