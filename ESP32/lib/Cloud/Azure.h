#pragma once

#include <Arduino.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>
#include <enums.h>

class AzureIoTHub
{
public:
    bool Initialize(char *securityKey, void (*event_queue_method)(CustomEvents), bool verbose = false);
    void HeartBeat();

private:
    const char *connectionString;
    IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
    int receiveContext = 0;

    //char static *resolveValue(char *text, char *key);
    //void static connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *user_context);
    //int static device_method_callback(const char *method_name, const unsigned char *payload, size_t size, unsigned char **response, size_t *response_size, void *userContextCallback);
};