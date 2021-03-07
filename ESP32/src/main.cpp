#include <main.h>

// TODO: Adding Support for BME680
// TODO: Adding Support for Device location
// TODO: Adding Support for handling wider Azure commands
// TODO: Improving the memory consumption & remove memory leaks

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  if (VERBOSE)
    printf("ESP-IDF version is : %s\r\n", esp_get_idf_version());

  EEPROM.begin(EEPROM_SIZE);
  ReadFlash();

  Oled.Initialize(true);

  Sensor.Initialize();

  initializeTimer();

  BluetoothLE.Initialize(BLE_DEVICE_NAME, &EnqueueEvent, VERBOSE);
  BluetoothLE.Setup();
  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP); // External PIN for triggering the advertise for BLE
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  wireless.Initialize(&EnqueueEvent, "GH-Home", VERBOSE);

  googleHome.Initialize(&EnqueueEvent, VERBOSE);

  if (HasValidWiFi())
    EnqueueEvent(CustomEvents::EVENT_WIFI_TRY_CONNECT);
  if (Sensor.CheckStatus())
  {
    if (Sensor.UpdateMeasurments())
      Oled.RefressSensorArea(
          Sensor.Measurments.cur_temperature,
          Sensor.Measurments.cur_humidity,
          Sensor.Measurments.cur_pressure,
          Sensor.Measurments.cur_airQuality);
  }
}

void loop()
{
  if (fireIoT)
  {
    azureIoT.HeartBeat();
    fireIoT = false;
  }

  if (tryStartBLE)
  {
    tryStartBLE = false;
    if (!isBLEadvertising)
      EnqueueEvent(CustomEvents::EVENT_BLE_TRY_START_ADV);
  }

  if (ble_advertize_timeOut == 0)
    EnqueueEvent(CustomEvents::EVENT_BLE_TRY_STOP_ADV);

  switch (DequeueEvent())
  {
  case CustomEvents::EVENT_BLE_CONNECTED:
    isBLEconnected = true;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_DISCONNECT:
    isBLEconnected = false;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_TRY_STOP_ADV:
    BluetoothLE.StopAdvertise();
    isBLEadvertising = false;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_TRY_START_ADV:
    BluetoothLE.StartAdvertise();
    isBLEadvertising = true;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_WIFI_START_SCAN:
    wireless.ScanNodes();
    break;
  case CustomEvents::EVENT_WIFI_SCAN_COMPLETED:
    BluetoothLE.SetSSIDs(wireless.SSIDs);
    break;
  case CustomEvents::EVENT_WIFI_TRY_CONNECT:
    wireless.TryConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
  case CustomEvents::EVENT_WIFI_CONNECTED:
    isWiFiconnected = true;
    BluetoothLE.SetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));
    ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
    UpdateStatus(true, true);
    ConfigureTime();
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT);
    EnqueueEvent(CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT);
    break;
  case CustomEvents::EVENT_WIFI_DISCONNECTED:
    isWiFiconnected = false;
    ssid = "";
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_WIFI_PASS_RECEIVED:
    BluetoothLE.StoreWiFiAuthentication();
    break;
  case CustomEvents::EVENT_WIFI_AZURE_KEY_RECEIVED:
    BluetoothLE.StoreAzureAuthentication();
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT);
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT:
    if (isWiFiconnected && HasValidHome())
      googleHome.TryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_CONNECTED:
    isHomeConnected = true;
    BluetoothLE.SetGoogleHomeName(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE:
    if (isHomeConnected)
      googleHome.NotifyTemperature((int)Sensor.Measurments.cur_temperature, azureIoT.GetLanguage());
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY:
    if (isHomeConnected)
      googleHome.NotifyHumidity((int)Sensor.Measurments.cur_humidity, azureIoT.GetLanguage());
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT:
    if (!HasValidAzure())
      return;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
      azureIoT.Initialize(
          (GetFlashValue(EEPROM_VALUE::Azure_IoT_Hub)).c_str(),
          &EnqueueEvent,
          (GetFlashValue(EEPROM_VALUE::User_Name)).c_str(),
          (GetFlashValue(EEPROM_VALUE::Device_Location)).c_str(),
          VERBOSE);
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_CONNECTED:
    isCloudconnected = true;
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_DISCONNECTED:
    isCloudconnected = false;
    UpdateStatus(true, true);
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

  if (readSenor)
  {
    time_t now;
    time(&now);
    if (Sensor.CheckStatus())
    {
      if (Sensor.UpdateMeasurments())
      {
        float temperature = Sensor.Measurments.cur_temperature;
        float humidity = Sensor.Measurments.cur_humidity;
        float pressure = Sensor.Measurments.cur_pressure;
        float airQuality = Sensor.Measurments.cur_airQuality;
        if (VERBOSE)
          printf(">> General Info %s Free Heap %d >> Temperature: %2.1fc(%2.1f-%2.1f) Humidity: %2.1f%%(%2.1f-%2.1f) Pressure: %2.2fatm AirQuality: %2.2f\r\n",
                 String(ctime(&now)).c_str(), ESP.getFreeHeap(),
                 temperature, Sensor.Measurments.min_temperature, Sensor.Measurments.max_temperature,
                 humidity, Sensor.Measurments.min_humidity, Sensor.Measurments.max_humidity,
                 pressure / 101325, airQuality);
        BluetoothLE.UpdateSensorValues(temperature, humidity, pressure);
        Oled.RefressSensorArea(temperature, humidity, pressure, airQuality);
      }
    }
    else
    {
      printf(">> General Info %s Free Heap %d\r\n",
             String(ctime(&now)).c_str(), ESP.getFreeHeap());
    }
    readSenor = false;
  }
}

void EnqueueEvent(CustomEvents newEvent)
{
  if (std::find(eventQueue.begin(), eventQueue.end(), newEvent) == eventQueue.end())
    eventQueue.push_back(newEvent);
}

CustomEvents DequeueEvent()
{
  if (eventQueue.size() == 0)
    return CustomEvents::EVENT_NONE;
  CustomEvents returnEvent = eventQueue.front();
  eventQueue.erase(eventQueue.begin());
  return returnEvent;
}

void ConfigureTime()
{
  const char *ntpServer = "pool.ntp.org";
  configTime(0, 0, ntpServer);
}

void UpdateStatus(bool BLE, bool OLED)
{
  if (OLED)
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
  if (BLE)
    BluetoothLE.UpdateConnectionStatus(isWiFiconnected, isHomeConnected, isCloudconnected);
}

void UpdateDailyReport(float temperature, float humidity, float pressure, float air_quality)
{
}