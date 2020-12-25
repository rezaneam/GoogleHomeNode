#include <main.h>

// TODO: Adding Support for starting advertise by external button
// TODO: Adding OLED display for Wireless and BLE status
// TODO: Adding Support for BME280 Sensor
// TODO: Adding Google Name & Keyword to the GATT
// TODO: Adding support for Google Home welcome message
// TODO: Adding Cloud service
// TODO: Adding IFTT service

SSD1306Wire Oled(OLED_Address, SDA_PIN, SCL_PIN);
bool hasBleEvent = false;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR handleExternalInterrupt()
{
  portENTER_CRITICAL_ISR(&mux);
  BLEstartAd();
  portEXIT_CRITICAL_ISR(&mux);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting App");

  Oled.init();
  Oled.flipScreenVertically();
  Oled.clear();
  Oled.setTextAlignment(TEXT_ALIGN_LEFT);
  Oled.setFont(ArialMT_Plain_16);
  //Oled.drawString(0, 0, "Initializing BLE");
  //Oled.drawIcon(OLEDDISPLAY_ICONS::BLE_ADVERTISING_ICON);

  Serial.println("Starting BLE");
  BLEinit(BLE_DEVICE_NAME, &hasBleEvent);
  BLEsetupAd();

  pinMode(BLE_ADVERTISE_ENABLE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BLE_ADVERTISE_ENABLE_PIN), handleExternalInterrupt, FALLING);
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
      WiFiConnect(BLEgetSSID(), BLEgetPassword());
    default:
      break;
    }
}
