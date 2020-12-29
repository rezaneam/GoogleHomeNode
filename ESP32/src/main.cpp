#include <main.h>

// TODO: Adding Google Name & Keyword to the GATT
// TODO: Adding support for Google Home welcome message
// TODO: Adding Cloud service
// TODO: Adding IFTT service
// TODO: Adding verbose flag for printing outputs
// TODO: Code Clean up - Phase 1 (minimize the logic in the main.cpp)
// TODO: Code Clean up - Phase 2 (organizing the process)
// TODO: Code Clean up - Phase 3 (using the event system)

#define SENSOR_READ_INTERVAL 10
BME280 Sensor = BME280();
hw_timer_t *sensorReadTimer = NULL;
portMUX_TYPE sensorReadtimerMux = portMUX_INITIALIZER_UNLOCKED;
bool readSenor = false;

SSD1306Wire Oled(OLED_Address, SDA_PIN, SCL_PIN);
bool hasBleEvent = false;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR handleExternalInterrupt()
{
  portENTER_CRITICAL_ISR(&mux);
  BLEstartAd();
  portEXIT_CRITICAL_ISR(&mux);
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
  Serial.begin(115200);
  EEPROM.begin(512);

  Serial.println("Starting App");

  Oled.init();
  //Oled.flipScreenVertically();
  Oled.clear();

  Sensor.begin();
  Sensor.setSampling(BME280::sensor_mode::MODE_FORCED,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_sampling::SAMPLING_X1,
                     BME280::sensor_filter::FILTER_OFF,
                     BME280::standby_duration::STANDBY_MS_1000);
  Sensor.takeForcedMeasurement();
  Oled.RefressSensorArea(Sensor.readTemperature(), Sensor.readHumidity(), Sensor.readPressure());
  initSensorReadTimer();

  Serial.println("Starting BLE");
  BLEinit(BLE_DEVICE_NAME, &hasBleEvent);
  BLEsetupAd();

  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);

  if (HasValidWiFi())
    WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
  else
    Serial.println("No WiFi is configured");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (hasBleEvent)
    switch (BLEreadEvent())
    {
    case ble_events::BLE_CONNECTED:
      Oled.BLEconnected(true);
      break;
    case ble_events::BLE_DISCONNECT:
      Oled.BLEconnected(false);
      break;
    case ble_events::BLE_STOPPED:
      Oled.BLEadvertising(false);
      break;
    case ble_events::BLE_STARTED:
      Oled.BLEadvertising(true);
      break;
    case ble_events::WIFI_START_SCAN:
      WiFiScanNodes();
      break;
    case ble_events::WIFI_CONNECTION_CHANGED:
      WiFiConnect(GetFlashValue(EEPROM_VALUE::WiFi_SSID), GetFlashValue(EEPROM_VALUE::WiFi_Password));
    case ble_events::WIFI_CONNECTED:
      Oled.WiFiconnected(true, GetFlashValue(EEPROM_VALUE::WiFi_SSID));
      break;
    case ble_events::WIFI_DISCONNECTED:
      Oled.WiFiconnected(false);
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
    Oled.RefressSensorArea(temperature, humidity, pressure);
    readSenor = false;
  }
}
