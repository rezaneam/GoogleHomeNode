#include <main.h>

// TODO: Moving BLE stuff to a seperate class
// TODO: Use connection string stored in the flash
// TODO: Adding verbose flag for printing outputs
// TODO: Code Clean up - Phase 1 (minimize the logic in the main.cpp)
// TODO: Code Clean up - Phase 2 (organizing the process)
// TODO: Code Clean up - Phase 3 (using the event system)

// ? Timer Settings
uint16_t ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
uint16_t timerCalls = 0;
void IRAM_ATTR handleExternalInterrupt()
{
  portENTER_CRITICAL_ISR(&externalPinmux);
  tryStartBLE = true;
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

  if (isBLEadvertising && !isBLEconnected)
    ble_advertize_timeOut--;

  portEXIT_CRITICAL_ISR(&sensorReadtimerMux);
}

void initializeTimer()
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
  if (VERBOSE)
    printf("ESP-IDF version is : %s\r\n", esp_get_idf_version());

  EEPROM.begin(EEPROM_SIZE);

  Oled.Initialize(true);

  Sensor.begin();
  Sensor.setSampling(BME280::sensor_mode::MODE_FORCED,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_filter::FILTER_OFF,
                     BME280::standby_duration::STANDBY_MS_1000);
  initializeTimer();

  BLEinit(BLE_DEVICE_NAME, &EnqueueEvent);
  BLEsetupAd();
  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP); // External PIN for triggering the advertise for BLE
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  wireless.Initialize(&EnqueueEvent, "GH-Home", VERBOSE);

  googleHome.Initialize(&EnqueueEvent, VERBOSE);

  if (HasValidWiFi())
    EnqueueEvent(CustomEvents::EVENT_WIFI_TRY_CONNECT);

  Sensor.takeForcedMeasurement();
  Oled.RefressSensorArea(Sensor.readTemperature(), Sensor.readHumidity(), Sensor.readPressure());
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
    BLEstopAd();
    isBLEadvertising = false;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_BLE_TRY_START_ADV:
    BLEstartAd();
    isBLEadvertising = true;
    ble_advertize_timeOut = BLE_ADVERTISE_TIMEOUT_MS;
    UpdateStatus(false, true);
    break;
  case CustomEvents::EVENT_WIFI_START_SCAN:
    wireless.ScanNodes();
    break;
  case CustomEvents::EVENT_WIFI_SCAN_COMPLETED:
    BLEsetSSIDs(wireless.SSIDs);
    break;
  case CustomEvents::EVENT_WIFI_TRY_CONNECT:
    wireless.TryConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
  case CustomEvents::EVENT_WIFI_CONNECTED:
    isWiFiconnected = true;
    BLEsetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));
    ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
    UpdateStatus(true, true);
    ConfigureTime();
    EnqueueEvent(CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT);
    EnqueueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT);
    break;
  case CustomEvents::EVENT_WIFI_DISCONNECTED:
    isWiFiconnected = false;
    ssid = "";
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT:
    if (isWiFiconnected && HasValidHome())
      googleHome.TryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_CONNECTED:
    BLEsetGoogleHomeName(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    UpdateStatus(true, true);
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_TEMPERATURE:
    if (isHomeConnected)
    {
      char buffer[200];
      sprintf(buffer, "The current temperature is %d", (int)Sensor.readTemperature());
      googleHome.Notify(GetFlashValue(EEPROM_VALUE::Google_Home_Name), buffer);
    }
    break;
  case CustomEvents::EVENT_GOOGLE_REPORT_HUMIDITY:
    if (isHomeConnected)
    {
      char buffer[200];
      sprintf(buffer, "The current humidity is %d", (int)Sensor.readHumidity());
      googleHome.Notify(GetFlashValue(EEPROM_VALUE::Google_Home_Name), buffer);
    }
    break;
  case CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT:
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
      azureIoT.Initialize(DEVICE_CONNECTION_STRING, &EnqueueEvent, VERBOSE);
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

    Sensor.takeForcedMeasurement();
    float temperature = Sensor.readTemperature();
    float humidity = Sensor.readHumidity();
    float pressure = Sensor.readPressure();
    if (VERBOSE)
      printf(">> General Info %s Free Heap %d >> Temperature: %2.1fc Humidity: %2.1f%% Pressure: %2.2fatm\r\n",
             String(ctime(&now)).c_str(), ESP.getFreeHeap(), temperature, humidity, pressure / 101325);
    UpdateSensorValues(temperature, humidity, pressure);
    Oled.RefressSensorArea(temperature, humidity, pressure);
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
    BLEupdateConnectionStatus(isWiFiconnected, isHomeConnected, isCloudconnected);
}