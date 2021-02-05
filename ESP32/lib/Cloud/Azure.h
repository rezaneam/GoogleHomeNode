#pragma once

#include <Arduino.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>
#include <enums.h>

class AzureIoTHub
{
public:
    bool Initialize(const char *securityKey, void (*event_queue_method)(CustomEvents), bool verbose = false);
    void HeartBeat();

private:
    IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
    int receiveContext = 0;
};