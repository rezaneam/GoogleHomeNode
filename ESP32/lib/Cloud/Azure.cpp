#include <Azure.h>

void static (*queueEvent)(const CustomEvents &);
void static (*google_notify)(std::string phrase, Languages language);
const char *device_location;
const char *username;
static bool isVerbose;
static char *azure_language = new char[3];

const char *USERID = "\"UserID\"";
const char *KEY = "\"Key\"";
const char *LANGUAGE = "\"Language\"";
const char *COMMAND = "COMMAND:";

void resolveValue(char *text, char const *key, char *result)
{
    char *pch = strstr(text, key);
    pch += strlen(key);
    pch = strchr(pch, '"');
    uint8_t i = 0;
    uint8_t pos = pch - text + 1;
    while (text[pos + i] != '"')
    {
        result[i] = text[pos + i];
        i++;
    }
    result[i] = '\0';
}

const char *resolveAzureKey(AzureKeys key, Languages lang)
{
    switch (lang)
    {
    case Languages::English:
        switch (key)
        {
        case AzureKeys::Temperature:
            return "temperature";
        case AzureKeys::Humidity:
            return "humidity";
        case AzureKeys::Pressure:
            return "pressure";
        case AzureKeys::AirQuality:
            return "air quality";
        case AzureKeys::Summary:
            return "summary";
        case AzureKeys::Report:
            return "report";
        }
        break;
    case Languages::Deutsch:
        switch (key)
        {
        case AzureKeys::Temperature:
            return "Temperatur";
        case AzureKeys::Humidity:
            return "Luftfeuchtigkeit";
        case AzureKeys::Pressure:
            return "Druck";
        case AzureKeys::AirQuality:
            return "Luftqualität";
        case AzureKeys::Summary:
            return "Zusammenfassung";
        case AzureKeys::Report:
            return "Bericht";
        }
        break;
    case Languages::Francais:
        switch (key)
        {
        case AzureKeys::Temperature:
            return "température";
        case AzureKeys::Humidity:
            return "humidité";
        case AzureKeys::Pressure:
            return "pression";
        case AzureKeys::AirQuality:
            return "qualité de l'air";
        case AzureKeys::Summary:
            return "résumé";
        case AzureKeys::Report:
            return "rapport";
        }
        break;
    }
    return "not supported";
}

Languages getLanguage()
{
    if (strstr(azure_language, "en"))
        return Languages::English;
    if (strstr(azure_language, "de"))
        return Languages::Deutsch;
    if (strstr(azure_language, "fr"))
        return Languages::Francais;
    return Languages::NotSupported;
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
        queueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_CONNECTED);
        if (isVerbose)
            LogInfo(">> Azure Log >> The device client is connected to iothub.");
    }
    else
    {
        queueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_DISCONNECTED);
        if (isVerbose)
            LogInfo(">> Azure Log >> The device client has been disconnected.");
    }
}

int static device_method_callback(
    const char *method_name,
    const unsigned char *payload, size_t size,
    unsigned char **response, size_t *response_size,
    void *userContextCallback)
{
    if (isVerbose)
        printf(">> Azure Log >> device_method_callback: \r\n", String(method_name));

    int result = 200;
    if (isVerbose)
        printf(">> Azure Log >> %s is called [%d] payload size\r\n", method_name, size);

    if (size > 320)
    {
        const char deviceMethodResponse[] = "{}";
        *response_size = sizeof(deviceMethodResponse) - 1;
        *response = (unsigned char *)malloc(*response_size);
        (void)memcpy(*response, deviceMethodResponse, *response_size);
        result = -1;
        return result;
    }

    if (strcmp(AzureMethodName, method_name) == 0)
    {
        char *buffer = new char[size + 1];
        //buffer = (char *)malloc(sizeof(char) * (size + 1));
        strncpy(buffer, (char *)payload, size);
        buffer[size] = '\0';
        if (isVerbose)
            printf(">> Azure Log >> Payload: %s\r\n", buffer);

        char *userId = new char[32];
        resolveValue(buffer, USERID, userId);

        resolveValue(buffer, LANGUAGE, azure_language);

        if (strlen(username) == 0 || strstr(userId, username)) // if user name doesn't exist or username matches
        {
            char *message = new char[size + 1];
            resolveValue(buffer, KEY, message);
            if (isVerbose)
            {
                printf(">> Azure Log >> UserId: %S\r\n", userId);
                printf(">> Azure Log >> Language: %S\r\n", azure_language);
                printf(">> Azure Log >> message is: %s\r\n", message);
            }

            bool isSummary = strstr(message, resolveAzureKey(AzureKeys::Summary, getLanguage()));
            if (!isSummary)
                isSummary = strstr(message, resolveAzureKey(AzureKeys::Report, getLanguage()));

            if (strstr(message, resolveAzureKey(AzureKeys::Temperature, getLanguage())))
                queueEvent(isSummary ? CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE_SUMMARY : CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE);

            else if (strstr(message, resolveAzureKey(AzureKeys::Humidity, getLanguage())))
                queueEvent(isSummary ? CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY_SUMMARY : CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY);

            else if (strstr(message, resolveAzureKey(AzureKeys::Pressure, getLanguage())))
                queueEvent(isSummary ? CustomEvents::EVENT_GOOGLE_REPORT_PRESSURE_SUMMARY : CustomEvents::EVENT_GOOGLE_REPORT_PRESSURE);

            else if (strstr(message, resolveAzureKey(AzureKeys::Pressure, getLanguage())))
                queueEvent(isSummary ? CustomEvents::EVENT_GOOGLE_REPORT_AIR_QUALITY_SUMMARY : CustomEvents::EVENT_GOOGLE_REPORT_AIR_QUALITY);

            else if (strstr(message, COMMAND)) // Has command in the message string.
            {

                uint8_t const command_len = strlen(COMMAND);
                uint8_t msg_len = strlen(message) - command_len + 1;
                char *phrase = new char[msg_len];
                strncpy(phrase, (char *)strstr(message, COMMAND) + command_len, msg_len);
                phrase[msg_len - 1] = '\0';
                printf(">> Azure Log >> COMMAND: %S\r\n", std::string(phrase).c_str());
                printf(">> Azure Log >> COMMAND: %S\r\n", phrase);
                google_notify(std::string(phrase),getLanguage());
            }

            delete[] userId;
            delete[] buffer;
            delete[] message;
            const char deviceMethodResponse[] = "{ \"Response\": \"Successful\" }";
            *response_size = sizeof(deviceMethodResponse) - 1;
            *response = (unsigned char *)malloc(*response_size);
            (void)memcpy(*response, deviceMethodResponse, *response_size);
            result = 200;
            return result;
        }
        delete[] userId;
        delete[] buffer;
    }
    // All other entries are ignored.
    const char deviceMethodResponse[] = "{}";
    *response_size = sizeof(deviceMethodResponse) - 1;
    *response = (unsigned char *)malloc(*response_size);
    (void)memcpy(*response, deviceMethodResponse, *response_size);
    result = -1;
    return result;
}

Languages AzureIoTHub::GetLanguage()
{
    return getLanguage();
}

bool AzureIoTHub::Initialize(
    const char *securityKey,
    void (*event_queue_method)(const CustomEvents &),
    void (*google_notify_method)(std::string phrase, Languages language),
    const char *userId,
    const char *devicelocation,
    bool verbose)
{
    isVerbose = verbose;
    queueEvent = event_queue_method;
    google_notify = google_notify_method;
    device_location = devicelocation;
    username = userId;

    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol = MQTT_Protocol;

    // Used to initialize IoTHub SDK subsystem
    IoTHub_Init();
    // Create the iothub handle here
    device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(securityKey, protocol);
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

void AzureIoTHub::Distroy()
{
    IoTHub_Deinit();
    device_ll_handle = NULL;
}