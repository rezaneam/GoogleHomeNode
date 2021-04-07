#include <main.h>

// TODO: Converting the Bluetooth Stack value to digits
// TODO: Showing the last status save
// TODO: Auto Detect different sensor address
// TODO: Adding Support for handling wider Azure commands
// TODO: Azure IoT Memory leak issue after de_init
// TODO: Improving the memory consumption & remove memory leaks
// TODO: Support for internet conenction
// TODO: Support for continues WiFi connectivity & pinging the gateway
// TODO: Investigate the break-down when Sensor failing
// TODO: Adding support for HDC1080

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  if (VERBOSE)
    printf("ESP-IDF version is : %s\r\n", esp_get_idf_version());

  EEPROM.begin(EEPROM_SIZE);
  ReadFlash();

  Oled.Initialize(true);

  if (OLED_SCL_PIN != SENSOR_SCL_PIN || OLED_SDA_PIN != SENSOR_SDA_PIN)
  {
    Wire1.begin(SENSOR_SDA_PIN, SENSOR_SCL_PIN, SDA_Frequency);
    Sensor.Initialize(Wire1, VERBOSE);
  }
  else
    Sensor.Initialize(Wire, VERBOSE);

  initializeTimer();

  BluetoothLE.Initialize(BLE_DEVICE_NAME, &EnqueueEvent, VERBOSE);
  BluetoothLE.Setup();
  pinMode(BLE_ADVERTISE_LED_PIN, OUTPUT);
  digitalWrite(BLE_ADVERTISE_LED_PIN, LOW);
  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP); // External PIN for triggering the advertise for BLE
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  wireless.Initialize(&EnqueueEvent, "GH-Home", (char *)VERBOSE);

  googleHome.Initialize(&EnqueueEvent, VERBOSE);

  if (HasValidWiFi())
    EnqueueEvent(CustomEvents::EVENT_WIFI_TRY_CONNECT);

  if (Sensor.CheckStatus())
    if (!Sensor.LoadState() & VERBOSE)
      printf("Failed to load the BSEC state\r\n");

  switch (Sensor.sensorType)
  {
  case SensorType::BMP280_Sensor:
    Oled.ShowMessage("BMP280", "Sensor", "TEMP + BAR");
    break;

  case SensorType::BME280_Sensor:
    Oled.ShowMessage("BME280", "Sensor", "TEMP + BAR + HUM");
    break;

  case SensorType::BME680_Sensor:
    Oled.ShowMessage("BME680", "Sensor", "TEMP + BAR + HUM + AQI");
    break;

  case SensorType::No_Sensor:
    Oled.ShowMessage("Error", "No Sensor found", "Check the connections");
    break;
  }
}

void loop()
{
  if (fireIoT)
  {
    if (connectionStatus.isInternetConnected)
      azureIoT.HeartBeat();
    fireIoT = false;
  }

  if (tryStartBLE)
  {
    tryStartBLE = false;
    if (!connectionStatus.isBLEAdvertising)
      EnqueueEvent(CustomEvents::EVENT_BLE_TRY_START_ADV);
  }

  if (ble_advertize_timeOut == 0)
    EnqueueEvent(CustomEvents::EVENT_BLE_TRY_STOP_ADV);

  switch (DequeueEvent())
  {
  case CustomEvents::EVENT_BLE_CONNECTED:
    connectionStatus.isBLEConnected = true;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_DISCONNECT:
    connectionStatus.isBLEConnected = false;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_TRY_STOP_ADV:
    BluetoothLE.StopAdvertise();
    digitalWrite(BLE_ADVERTISE_LED_PIN, LOW);
    connectionStatus.isBLEAdvertising = false;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_TRY_START_ADV:
    BluetoothLE.StartAdvertise();
    connectionStatus.isBLEAdvertising = true;
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
    break;
  case CustomEvents::EVENT_WIFI_FORCE_CONNECT:
    wireless.TryConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password), true);
    break;
  case CustomEvents::EVENT_WIFI_TRY_DISCONNECT:
    wireless.Disconnect();
    EnqueueEvent(CustomEvents::EVENT_WIFI_DISCONNECTED);
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_DISCONNECT);
    break;
  case CustomEvents::EVENT_WIFI_CONNECTED:
    connectionStatus.isWiFiConnected = true;
    BluetoothLE.SetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));
    ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
    UpdateStatus(true, true);
    ConfigureTime();
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT);
    EnqueueEvent(CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT);
    break;
  case CustomEvents::EVENT_WIFI_DISCONNECTED:
    connectionStatus.isWiFiConnected = false;
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
    if (connectionStatus.isWiFiConnected && HasValidHome())
      googleHome.TryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_CONNECTED:
    connectionStatus.isHomeConnected = true;
    BluetoothLE.SetGoogleHomeName(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE:
    UpdateStatus(false, true, true);
    if (connectionStatus.isHomeConnected)
      googleHome.NotifyTemperature((int)Sensor.Measurments.cur_temperature, azureIoT.GetLanguage());
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE_SUMMARY:
    UpdateStatus(false, true, true);
    if (connectionStatus.isHomeConnected)
      googleHome.NotifyTemperatureSummary(
          (int)Sensor.Measurments.ave_temperature,
          (int)Sensor.Measurments.min_temperature,
          (int)Sensor.Measurments.max_temperature,
          azureIoT.GetLanguage());
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY:
    UpdateStatus(false, true, true);
    if (connectionStatus.isHomeConnected)
      googleHome.NotifyHumidity((int)Sensor.Measurments.cur_humidity, azureIoT.GetLanguage());
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY_SUMMARY:
    UpdateStatus(false, true, true);
    if (connectionStatus.isHomeConnected)
      googleHome.NotifyHumiditySummary(
          (int)Sensor.Measurments.ave_humidity,
          (int)Sensor.Measurments.min_humidity,
          (int)Sensor.Measurments.max_humidity,
          azureIoT.GetLanguage());
    UpdateStatus(false, true);
    break;
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT:
    if (!HasValidAzure() | !connectionStatus.isWiFiConnected)
      break;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
      bool result = azureIoT.Initialize(
          (GetFlashValue(EEPROM_VALUE::Azure_IoT_Hub)).c_str(),
          &EnqueueEvent,
          (GetFlashValue(EEPROM_VALUE::User_Name)).c_str(),
          (GetFlashValue(EEPROM_VALUE::Device_Location)).c_str(),
          VERBOSE);
      EnqueueEvent(result ? CustomEvents::EVENT_AZURE_IOT_HUB_CONNECTED : CustomEvents::EVENT_AZURE_IOT_HUB_DISCONNECTED);
    }
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_CONNECTED:
    connectionStatus.isCloudConnected = true;
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_TRY_DISCONNECT:
    azureIoT.Distroy();
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_DISCONNECTED);
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_DISCONNECTED:
    connectionStatus.isCloudConnected = false;
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_FACTORY_RESET:
    Oled.ShowMessage("Factory Reset", "Ereasing...");
    if (VERBOSE)
      printf("Factory Reset triggered by user\r\n");
    EraseFlash();
    delay(5000);
    ESP.restart();
    break;
  case CustomEvents::EVENT_FACTORY_RESET_SAFE:
    Oled.ShowMessage("Factory Reset", "Safe earsing...");
    if (VERBOSE)
      printf("Factory Reset [Safe] triggered by user\r\n");
    EraseFlash(true);
    delay(5000);
    ESP.restart();
    break;
  case CustomEvents::EVENT_RESTART:
    Oled.ShowMessage("Restarting...", "Please wait.");
    if (VERBOSE)
      printf("Rebooting triggered by user\r\n");
    delay(5000);
    ESP.restart();
    break;
  case CustomEvents::EVENT_LOW_RAM_RESTART:
    Oled.ShowMessage("Restarting...", "Low Memory!");
    if (VERBOSE)
      printf("Rebooting due to low memeory\r\n");
    delay(5000);
    ESP.restart();
    break;
  default:
    break;
  }
  if (readSenor)
  {
    uint8_t freeHeap = 100 * ESP.getFreeHeap() / ESP.getHeapSize();
    char buffer[100] = "";
    connectionStatus.isWiFiConnected = wireless.IsConnected();
    if (connectionStatus.isWiFiConnected)
    {
      connectionStatus.isInternetConnected = wireless.IsOnline();
      connectionStatus.isCloudConnected = connectionStatus.isInternetConnected;
      if (connectionStatus.isInternetConnected)
      {
        getTimeString(&buffer[0]);
      }
    }
    else
    {
      connectionStatus.isInternetConnected = false;
      connectionStatus.isHomeConnected = false;
      if (HasValidWiFi())
        EnqueueEvent(CustomEvents::EVENT_WIFI_TRY_CONNECT);
    }

    UpdateStatus(true, true);
    if (Sensor.CheckStatus())
    {

      if (Sensor.UpdateMeasurments())
      {
        float temperature = Sensor.Measurments.cur_temperature;
        float humidity = Sensor.Measurments.cur_humidity;
        float pressure = Sensor.Measurments.cur_pressure;
        float airQuality = Sensor.Measurments.cur_airQuality;
        if (VERBOSE)
        {

          printf(">> Free Heap %d%% %s [%d]", freeHeap, buffer, Sensor.Measurments.total_readgings);
          printf(" Temperature: %2.1fc(%2.1f-%2.1f)[%2.1f]",
                 temperature, Sensor.Measurments.min_temperature, Sensor.Measurments.max_temperature, Sensor.Measurments.ave_temperature);
          printf(" Pressure: %2.2fatm(%2.2f-%2.2f)[%2.2f]",
                 pressure / 101325, Sensor.Measurments.min_pressure / 101325, Sensor.Measurments.max_pressure / 101325, Sensor.Measurments.ave_pressure / 101325);
          if (humidity >= 0)
            printf(" Humidity: %2.1f%%(%2.1f-%2.1f)[%2.1f] ",
                   humidity, Sensor.Measurments.min_humidity, Sensor.Measurments.max_humidity, Sensor.Measurments.ave_humidity);
          if (airQuality >= 0)
          {
            printf("AirQuality: %2.1f(%2.1f-%2.1f)[%2.1f]",
                   airQuality, Sensor.Measurments.min_air_quality, Sensor.Measurments.max_air_quality, Sensor.Measurments.ave_airQuality);
            printf(" VOC: %2.1f%% CO2: %2.1f",
                   Sensor.Measurments.VOC, Sensor.Measurments.CO2);
          }
          printf("\r\n");
        }
        BluetoothLE.UpdateSensorValues(temperature, humidity, pressure, airQuality);
      }
    }
    else
    {
      if (VERBOSE)
        printf(">> Free Heap %d%% %s\r\n", buffer, freeHeap);
    }
    readSenor = false;

    if (freeHeap <= MinimumAllowHeapSize)
      EnqueueEvent(CustomEvents::EVENT_LOW_RAM_RESTART);
  }
  if (secondFlag)
  {
    RefreshOLED();
    Sensor.Run();
    storeBSECstateCountdown--;
    if (storeBSECstateCountdown == 0)
      storeBSECstateCountdown = Sensor.StoreState() ? UPDATE_BSEC_STATE_INTERVAL_SEC : RETRY_UPDATE_BSEC_STATE_INTERVAL_SEC;

    secondFlag = false;
  }
}

void getTimeString(char *buffer)
{
  time_t now;
  time(&now);
  tm *ltm = localtime(&now);

  sprintf(buffer, "%d/%d/%d %d:%d:%d",
          ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

  if (ltm->tm_min == START_OF_DAY_MIN & ltm->tm_hour == START_OF_DAY_HOUR)
    Sensor.ResetReport();
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
  HTTPClient http;
  const char *ntpServer = "pool.ntp.org";
  const char *timeApiUrl = "http://worldtimeapi.org/api/ip";

  http.begin(timeApiUrl);
  int httpCode = http.GET();

  if (httpCode != 200)
  {
    printf("Unable to fetch the time info\r\n");
    configTime(0, 0, ntpServer);
    return;
  }
  String response = http.getString();
  long offset = response.substring(response.indexOf("\"raw_offset\":") + 13, response.indexOf(",\"timezone\":")).toFloat();
  long daylight_offset = response.substring(response.indexOf("\"dst_offset\":") + 13, response.indexOf(",\"dst_until\":")).toFloat();
  http.end();

  configTime(offset, daylight_offset, ntpServer);
}

void UpdateStatus(bool BLE, bool OLED, bool isNotifying)
{
  if (OLED)
    Oled.ReferessStatusArea(connectionStatus, ssid, isNotifying);
  if (BLE)
    BluetoothLE.UpdateConnectionStatus(connectionStatus);
}

void RefreshOLED()
{

  changeDisplayTimeout--;
  if (changeDisplayTimeout > 0)
    return;
  if (Sensor.sensorType == SensorType::No_Sensor)
    return;
  bool isBME680 = Sensor.sensorType == SensorType::BME680_Sensor;
  bool isBME280 = Sensor.sensorType == SensorType::BME280_Sensor;
  switch (Oled.CurrentShow)
  {
  case DisplayStatus::AllSensors:
    Oled.ShowMSummary(
        Sensor.Measurments.ave_temperature,
        Sensor.Measurments.min_temperature,
        Sensor.Measurments.max_temperature,
        DisplayStatus::TemperatureSensor);
    changeDisplayTimeout = 2;
    return;
  case DisplayStatus::TemperatureSensor:
    Oled.ShowMSummary(
        Sensor.Measurments.ave_pressure,
        Sensor.Measurments.min_pressure,
        Sensor.Measurments.max_pressure,
        DisplayStatus::PressureSensor);
    changeDisplayTimeout = 2;
    return;
  case DisplayStatus::PressureSensor:
    if (isBME280 | isBME680)
    {
      Oled.ShowMSummary(
          Sensor.Measurments.ave_humidity,
          Sensor.Measurments.min_humidity,
          Sensor.Measurments.max_humidity,
          DisplayStatus::HumiditySensor);
      changeDisplayTimeout = 2;
      return;
    }
    break;
  case DisplayStatus::HumiditySensor:
    if (!isBME680)
      break;
    Oled.ShowMSummary(
        Sensor.Measurments.calibrationStatus >= SensorCalibrationStatus::MEDIUM_ACCURACY ? Sensor.Measurments.ave_airQuality : Sensor.Measurments.gas_resistance,
        Sensor.Measurments.min_air_quality,
        Sensor.Measurments.max_air_quality,
        DisplayStatus::AirQualitySensor,
        Sensor.Measurments.calibrationStatus);
    changeDisplayTimeout = 2;
    return;
    break;

  case DisplayStatus::AirQualitySensor:
    if (Sensor.Measurments.calibrationStatus >= SensorCalibrationStatus::MEDIUM_ACCURACY)
    {
      Oled.ShowAirQualitySummary(Sensor.Measurments.cur_airQuality, Sensor.Measurments.VOC);
      changeDisplayTimeout = 2;
      return;
    }
    break;
  }

  if (Oled.CurrentShow != DisplayStatus::Time && connectionStatus.isWiFiConnected)
  {
    time_t now;
    time(&now);
    tm *ltm = localtime(&now);
    Oled.ShowDateTime(ltm);
    changeDisplayTimeout = 2;
    return;
  }
  Oled.RefressSensorArea(
      Sensor.Measurments.cur_temperature,
      Sensor.Measurments.cur_humidity,
      Sensor.Measurments.cur_pressure,
      Sensor.Measurments.cur_airQuality,
      Sensor.Measurments.calibrationStatus);
  changeDisplayTimeout = 4;
}