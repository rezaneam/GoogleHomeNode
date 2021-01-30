#include <main.h>

// TODO: Adding Google Name & Keyword to the GATT
// TODO: Adding support for Google Home welcome message
// TODO: Adding Cloud service
// TODO: Adding Cloud icon on Oled
// TODO: Adding IFTT service
// TODO: Adding verbose flag for printing outputs
// TODO: Code Clean up - Phase 1 (minimize the logic in the main.cpp)
// TODO: Code Clean up - Phase 2 (organizing the process)
// TODO: Code Clean up - Phase 3 (using the event system)

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

// ? Azure IoT
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <iothubtransportmqtt.h>

/* Define several constants/global variables */
static const char *connectionString = DEVICE_CONNECTION_STRING;
static bool g_continueRunning = true; // defines whether or not the device maintains its IoT Hub connection after sending (think receiving messages from the cloud)

const char *quit_msg = "quit";
const char *exit_msg = "exit";

IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;

int receiveContext = 0;
bool fireIoT = false;
/* -- receive_message_callback --
 * Callback method which executes upon receipt of a message originating from the IoT Hub in the cloud. 
 * Note: Modifying the contents of this method allows one to command the device from the cloud. 
 */
static IOTHUBMESSAGE_DISPOSITION_RESULT receive_message_callback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
  LogInfo("A message received from IoT Hub");
  int *counter = (int *)userContextCallback;
  const unsigned char *buffer;
  size_t size;
  const char *messageId;

  // Message properties
  if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
  {
    messageId = "<null>";
  }

  // Message content
  if (IoTHubMessage_GetByteArray(message, (const unsigned char **)&buffer, &size) != IOTHUB_MESSAGE_OK)
  {
    LogInfo("unable to retrieve the message data\r\n");
  }
  else
  {
    LogInfo("Received Message [%d]\r\n Message ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", *counter, messageId, (int)size, buffer, (int)size);
    // If we receive the word 'quit' then we stop running
    if (size == (strlen(quit_msg) * sizeof(char)) && memcmp(buffer, quit_msg, size) == 0)
    {
      g_continueRunning = false;
    }
  }

  /* Some device specific action code goes here... */
  (*counter)++;
  return IOTHUBMESSAGE_ACCEPTED;
}

/* -- connection_status_callback --
 * Callback method which executes on receipt of a connection status message from the IoT Hub in the cloud.
 */
static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *user_context)
{
  (void)reason;
  (void)user_context;
  // This sample DOES NOT take into consideration network outages.
  if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
  {
    LogInfo("The device client is connected to iothub.");
  }
  else
  {
    LogInfo("The device client has been disconnected.");
  }
}

static int device_method_callback(const char *method_name, const unsigned char *payload, size_t size, unsigned char **response, size_t *response_size, void *userContextCallback)
{
  Serial.println("device_method_callback: " + String(method_name));

  int result = 200;
  printf("%s is called [%d] payload size\r\n", method_name, size);
  if (strcmp("Google Home", method_name) == 0)
  {
    char *buffer;
    buffer = (char *)malloc(sizeof(char) * (size + 1));
    strncpy(buffer, (char *)payload, size);
    buffer[size] = '\0';
    printf("Payload: %s\r\n", buffer);

    char *message = resolveValue(buffer, "\"Key\"");

    if (strstr(message, "temperature"))

      activeEvent = CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE;
    else if (strstr(message, "humidity"))
      activeEvent = CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY;

    printf("message is: %s\r\n", message);
    free(buffer);
    const char deviceMethodResponse[] = "{ \"Response\": \"1HGCM82633A004352\" }";
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

// ? Azure IoT

// ? Timer Settings
uint64_t timerCalls = 0;
void IRAM_ATTR handleExternalInterrupt()
{
  portENTER_CRITICAL_ISR(&externalPinmux);
  BLEstartAd();
  portEXIT_CRITICAL_ISR(&externalPinmux);
}

void IRAM_ATTR onReadSensor()
{
  portENTER_CRITICAL_ISR(&sensorReadtimerMux);
  timerCalls++;
  if (timerCalls % 50 == 0)
    fireIoT = true;
  if (timerCalls == SENSOR_READ_INTERVAL * 1000)
  { // Read sensor and update screen
    readSenor = true;
    timerCalls = 0;
  }
  portEXIT_CRITICAL_ISR(&sensorReadtimerMux);
}

void initSensorReadTimer()
{
  sensorReadTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(sensorReadTimer, &onReadSensor, true);
  timerAlarmWrite(sensorReadTimer, 1000, true);
  timerAlarmEnable(sensorReadTimer);
}

// ? Timer Settings

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("ESP-IDF version is : " + String(esp_get_idf_version()));

  EEPROM.begin(EEPROM_SIZE);

  Serial.println("Starting App");

  Oled.Initialize(true);

  Sensor.begin();
  Sensor.setSampling(BME280::sensor_mode::MODE_FORCED,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_filter::FILTER_OFF,
                     BME280::standby_duration::STANDBY_MS_1000);
  initSensorReadTimer();
  Serial.println("Starting BLE");
  BLEinit(BLE_DEVICE_NAME, &activeEvent);
  BLEsetupAd();

  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  if (HasValidWiFi())
  {
    isWiFiconnected = WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    if (isWiFiconnected)
    {
      BLEsetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));

      if (HasValidHome())
        NotifierTryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));

      const char *ntpServer = "pool.ntp.org";
      configTime(0, 0, ntpServer);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo))
      {
        // ! Azure IoT
        isCloudconnected = InitIoT();
      }
    }
  }

  Sensor.takeForcedMeasurement();
  Oled.RefressSensorArea(Sensor.readTemperature(), Sensor.readHumidity(), Sensor.readPressure());
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (fireIoT && device_ll_handle != NULL)
  {
    IoTHubDeviceClient_LL_DoWork(device_ll_handle);
    fireIoT = false;
  }

  if (activeEvent != CustomEvents::EVENT_NONE)
  {
    switch (activeEvent)
    {
    case CustomEvents::EVENT_BLE_CONNECTED:
      isBLEconnected = true;
      isBLEadvertising = false;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_BLE_DISCONNECT:
      isBLEconnected = false;
      isBLEadvertising = true;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_BLE_STOPPED:
      NimBLEDevice::stopAdvertising();
      isBLEconnected = false;
      isBLEadvertising = false;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_BLE_STARTED:
      NimBLEDevice::startAdvertising();
      isBLEconnected = false;
      isBLEadvertising = true;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_WIFI_START_SCAN:
      WiFiScanNodes();
      break;
    case CustomEvents::EVENT_WIFI_TRY_CONNECT:
      WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    case CustomEvents::EVENT_WIFI_CONNECTED:
      isWiFiconnected = true;
      ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_WIFI_DISCONNECTED:
      isWiFiconnected = false;
      ssid = "";
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
      break;
    case CustomEvents::EVENT_GOOGLE_HOME_NAME:
      break;
    case CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE:
      if (isHomeConnected)
      {
        char buffer[200];
        sprintf(buffer, "The current temperature is %d", (int)Sensor.readTemperature());
        NotifierNotify(GetFlashValue(EEPROM_VALUE::Google_Home_Name), buffer);
      }
      break;
    case CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY:
      if (isHomeConnected)
      {
        char buffer[200];
        sprintf(buffer, "The current humidity is %d", (int)Sensor.readHumidity());
        NotifierNotify(GetFlashValue(EEPROM_VALUE::Google_Home_Name), buffer);
      }
      break;
    case CustomEvents::EVENT_FACTORY_RESET:
      Oled.ShowRestMessage("Factory Reset");
      EraseFlash();
      delay(5000);
      ESP.restart();
      break;
    case CustomEvents::EVENT_FACTORY_RESET_SAFE:
      Oled.ShowRestMessage("Factory Reset [safe]");
      EraseFlash(true);
      delay(5000);
      ESP.restart();
      break;
    case CustomEvents::EVENT_RESTART:
      Oled.ShowRestMessage("Restarting...");
      delay(5000);
      ESP.restart();
      break;
    default:
      break;
    }
    activeEvent = CustomEvents::EVENT_NONE;
  }

  if (readSenor)
  {
    time_t now;
    time(&now);

    Serial.println(">> Info: " + String(ctime(&now)) + " Free heap is " + String(ESP.getFreeHeap()));
    Sensor.takeForcedMeasurement();
    float temperature = Sensor.readTemperature();
    float humidity = Sensor.readHumidity();
    float pressure = Sensor.readPressure();
    UpdateSensorValues(temperature, humidity, pressure);
    Oled.RefressSensorArea(temperature, humidity, pressure);
    readSenor = false;
  }
}

bool NotifierTryConnect(std::string deviceName)
{
  if (isHomeConnected)
    return true;

  Serial.println("connecting to Google Home: " + String(deviceName.c_str()));
  if (googleHomeNotifier.device(deviceName.c_str(), "en") != true)
  {
    Serial.println(googleHomeNotifier.getLastError());
    return false;
  }
  else
  {
    isHomeConnected = true;
    Serial.println("found Google Home(" + googleHomeNotifier.getIPAddress().toString() + ":" + String(googleHomeNotifier.getPort()) + ")");
    if (googleHomeNotifier.notify(Notifier_WELCOME_MSG) != true)
    {
      Serial.println(googleHomeNotifier.getLastError());
      return false;
    }
    isHomeConnected = true;
  }

  return true;
}

bool NotifierNotify(std::string deviceName, std::string message)
{
  if (!NotifierTryConnect(deviceName))
    return false;
  if (googleHomeNotifier.notify(message.c_str()) != true)
  {
    Serial.println(googleHomeNotifier.getLastError());
    return false;
  }
  return true;
}

bool InitIoT()
{
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
  if (IoTHubClient_LL_SetMessageCallback(device_ll_handle, receive_message_callback, &receiveContext) != IOTHUB_CLIENT_OK)
  {
    LogInfo("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!");
    return false;
  }

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