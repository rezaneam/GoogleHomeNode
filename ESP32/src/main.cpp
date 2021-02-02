#include <main.h>

// TODO: Moving BLE stuff to a seperate class
// TODO: Use connection string stored in the flash
// TODO: Adding verbose flag for printing outputs
// TODO: Code Clean up - Phase 1 (minimize the logic in the main.cpp)
// TODO: Code Clean up - Phase 2 (organizing the process)
// TODO: Code Clean up - Phase 3 (using the event system)

// ? Timer Settings
uint64_t timerCalls = 0;
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
  BLEinit(BLE_DEVICE_NAME, &EnqueueEvent);
  BLEsetupAd();

  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  wireless.Initialize(&EnqueueEvent, "GH Home", VERBOSE);
  googleHome.Initialize(&EnqueueEvent, VERBOSE);

  if (HasValidWiFi())
  {
    isWiFiconnected = wireless.TryConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    if (isWiFiconnected)
    {
      BLEsetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));

      if (HasValidHome())
        googleHome.TryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));

      const char *ntpServer = "pool.ntp.org";
      configTime(0, 0, ntpServer);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo))
      {
        // ! Azure IoT
        isCloudconnected = azureIoT.Initialize(DEVICE_CONNECTION_STRING, &EnqueueEvent, VERBOSE);
      }
    }
  }

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

  if (isBLEadvertising && !BLEgetAdvertiseStatus())
    EnqueueEvent(CustomEvents::EVENT_BLE_STOPPED);

  switch (DequeueEvent())
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
    //NimBLEDevice::stopAdvertising();
    Serial.println("BLE stopped advertising.");
    isBLEconnected = false;
    isBLEadvertising = false;
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
    break;
  case CustomEvents::EVENT_BLE_TRY_START_ADV:
    BLEstartAd();
    //NimBLEDevice::startAdvertising();
    isBLEconnected = false;
    isBLEadvertising = true;
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
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
    BLEupdateConnectionStatus(isWiFiconnected, isHomeConnected, isCloudconnected);
    ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
    break;
  case CustomEvents::EVENT_WIFI_DISCONNECTED:
    isWiFiconnected = false;
    BLEupdateConnectionStatus(isWiFiconnected, isHomeConnected, isCloudconnected);
    ssid = "";
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_NAME:
    if (isWiFiconnected && HasValidHome())
      googleHome.TryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    break;
  case CustomEvents::EVENT_GOOGLE_HOME_CONNECTED:
    BLEsetGoogleHomeName(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
    BLEupdateConnectionStatus(isWiFiconnected, isHomeConnected, isCloudconnected);
    Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid, isCloudconnected);
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
