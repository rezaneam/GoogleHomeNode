#include <Azure.h>

void static (*queueEvent)(CustomEvents);
bool static isVerbose;

char *resolveValue(char *text, char *key)
{
    char *pch = strstr(text, key);
    uint8_t pos1, pos2;
    pch += strlen(key);
    pch = strchr(pch, '"');
    pos1 = pch - text + 1;
    pch = strchr(pch + 1, '"');
    pos2 = pch - text + 1;
    char *returnValue = new char[pos2 - pos1 - 1];

    for (size_t i = 0; i < (pos2 - pos1 - 1); i++)
        returnValue[i] = text[pos1 + i];
    returnValue[pos2 - pos1 - 1] = '\0';

    return returnValue;
}

/* -- connection_status_callback --
 * Callback method which executes on receipt of a connection status message from the IoT Hub in the cloud.
 */
void static connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *user_context)
{
    (void)reason;
    (void)user_context;
    // This sample DOES NOT take into consideration network outages.
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        if (isVerbose)
            LogInfo("The device client is connected to iothub.");
    }
    else
    {
        if (isVerbose)
            LogInfo("The device client has been disconnected.");
    }
}

int static device_method_callback(const char *method_name, const unsigned char *payload, size_t size, unsigned char **response, size_t *response_size, void *userContextCallback)
{
    if (isVerbose)
        printf("device_method_callback: \r\n", String(method_name));

    int result = 200;
    if (isVerbose)
        printf("%s is called [%d] payload size\r\n", method_name, size);
    if (strcmp("Google Home", method_name) == 0)
    {
        char *buffer;
        buffer = (char *)malloc(sizeof(char) * (size + 1));
        strncpy(buffer, (char *)payload, size);
        buffer[size] = '\0';
        if (isVerbose)
            printf("Payload: %s\r\n", buffer);

        char *message = resolveValue(buffer, "\"Key\"");

        if (strstr(message, "temperature"))
            queueEvent(CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE);
        else if (strstr(message, "humidity"))
            queueEvent(CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY);

        if (isVerbose)
            printf("message is: %s\r\n", message);
        free(buffer);
        const char deviceMethodResponse[] = "{ \"Response\": \"GoogleHomeResponse\" }";
        *response_size = sizeof(deviceMethodResponse) - 1;
        *response = (unsigned char *)malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);
        result = 200;
        return result;
    }
    // All other entries are ignored.
    const char deviceMethodResponse[] = "{ }";
    *response_size = sizeof(deviceMethodResponse) - 1;
    *response = (unsigned char *)malloc(*response_size);
    (void)memcpy(*response, deviceMethodResponse, *response_size);
    result = -1;
    return result;
}

bool AzureIoTHub::Initialize(char *securityKey, void (*event_queue_method)(CustomEvents), bool verbose)
{
    isVerbose = verbose;
    queueEvent = event_queue_method;
    connectionString = securityKey;

    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;

    // Used to initialize IoTHub SDK subsystem
    IoTHub_Init();
    // Create the iothub handle here
    device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, protocol);
    LogInfo("Creating IoTHub Device handle.");

    if (device_ll_handle == NULL)
    {
        LogInfo("Error AZ002: Failure creating Iothub device. Hint: Check you connection string.");
        return false;
    }

    // Set any option that are neccessary.
    // For available options please see the iothub_sdk_options.md documentation in the main C SDK
    // turn off diagnostic sampling
    int diag_off = 0;
    IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE, &diag_off);

    // Setting the Trusted Certificate.
    IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_TRUSTED_CERT, certificates);

    //Setting the auto URL Encoder (recommended for MQTT). Please use this option unless
    //you are URL Encoding inputs yourself.
    //ONLY valid for use with MQTT
    bool urlEncodeOn = true;
    IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);
    /* Setting Message call back, so we can receive Commands. */

    // Setting connection status callback to get indication of connection to iothub
    if (IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle, connection_status_callback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("ERROR: IoTHubDeviceClient_LL_SetConnectionStatusCallback..........FAILED!");
        return false;
    }

    if (IoTHubDeviceClient_LL_SetDeviceMethodCallback(device_ll_handle, device_method_callback, NULL) != IOTHUB_CLIENT_OK)
    {
        LogInfo("ERROR: IoTHubDeviceClient_LL_SetDeviceMethodCallback..........FAILED!");
        return false;
    }

    LogInfo("IoT Device created.");

    return true;
}

void AzureIoTHub::HeartBeat()
{
    if (device_ll_handle != NULL)
        IoTHubDeviceClient_LL_DoWork(device_ll_handle);
}