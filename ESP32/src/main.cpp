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

void IRAM_ATTR handleExternalInterrupt()
{
  portENTER_CRITICAL_ISR(&externalPinmux);
  BLEstartAd();
  portEXIT_CRITICAL_ISR(&externalPinmux);
}

void IRAM_ATTR onReadSensor()
{
  portENTER_CRITICAL_ISR(&sensorReadtimerMux);
  // Read sensor and update screen
  readSenor = true;
  portEXIT_CRITICAL_ISR(&sensorReadtimerMux);
}

void initSensorReadTimer()
{
  sensorReadTimer = timerBegin(1, 80, true);
  timerAttachInterrupt(sensorReadTimer, &onReadSensor, true);
  timerAlarmWrite(sensorReadTimer, 1000000 * SENSOR_READ_INTERVAL, true);
  timerAlarmEnable(sensorReadTimer);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("ESP-IDF version is : " + String(esp_get_idf_version()));
  Serial.println("Free heap is " + String(ESP.getFreeHeap()));
  EEPROM.begin(EEPROM_SIZE);

  Serial.println("Starting App");

  Oled.initialize();

  Sensor.begin();
  Sensor.setSampling(BME280::sensor_mode::MODE_FORCED,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_filter::FILTER_OFF,
                     BME280::standby_duration::STANDBY_MS_1000);
  initSensorReadTimer();
  Serial.println("Free heap is " + String(ESP.getFreeHeap()));
  Serial.println("Starting BLE");
  BLEinit(BLE_DEVICE_NAME, &hasBleEvent);
  BLEsetupAd();

  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);
  Serial.println("Free heap is " + String(ESP.getFreeHeap()));
  if (HasValidWiFi())
  {
    WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    BLEsetSSID(GetFlashValue(EEPROM_VALUE::WiFi_SSID));

    if (HasValidHome())
      NotifierTryConnect(GetFlashValue(EEPROM_VALUE::Google_Home_Name));
  }

  Sensor.takeForcedMeasurement();
  Oled.RefressSensorArea(Sensor.readTemperature(), Sensor.readHumidity(), Sensor.readPressure());
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (hasBleEvent)
    switch (BLEreadEvent())
    {
    case BLEEvents::BLE_CONNECTED:
      isBLEconnected = true;
      isBLEadvertising = false;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::BLE_DISCONNECT:
      isBLEconnected = false;
      isBLEadvertising = true;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::BLE_STOPPED:
      NimBLEDevice::stopAdvertising();
      isBLEconnected = false;
      isBLEadvertising = false;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::BLE_STARTED:
      NimBLEDevice::startAdvertising();
      isBLEconnected = false;
      isBLEadvertising = true;
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::WIFI_START_SCAN:
      WiFiScanNodes();
      break;
    case BLEEvents::WIFI_CONNECTION_CHANGED:
      WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    case BLEEvents::WIFI_CONNECTED:
      isWiFiconnected = true;
      ssid = GetFlashValue(EEPROM_VALUE::WiFi_SSID);
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::WIFI_DISCONNECTED:
      isWiFiconnected = false;
      ssid = "";
      Oled.ReferessStatusArea(isBLEadvertising, isBLEconnected, isHomeConnected, isWiFiconnected, ssid);
      break;
    case BLEEvents::GOOGLE_HOME_NAME:
      break;
    default:
      break;
    }

  if (readSenor)
  {
    Sensor.takeForcedMeasurement();
    float temperature = Sensor.readTemperature();
    float humidity = Sensor.readHumidity();
    float pressure = Sensor.readPressure();
    UpdateSensorValues(temperature, humidity, pressure);
    // if (isHomeConnected)
    // {
    //   sprintf(messageBuffer, "Temperature is %d degrees and it is %d %% humide. BTW, the pressure is %.1f. atm", (int16_t)temperature, (int16_t)humidity, pressure / 101325);
    //   Serial.println(messageBuffer);
    //   ghn.notify(messageBuffer);
    //   Serial.println("Free heap is " + String(ESP.getFreeHeap()));
    // }
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