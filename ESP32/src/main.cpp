#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <string>
#include <SSD1306.h>
#include <Wireless.h>

#define OLED_SDA_pin 5
#define OLED_SCL_pin 4
#define Frequency 400000
#define OLED_Address 0x3c

SSD1306Wire Oled(OLED_Address, OLED_SDA_pin, OLED_SCL_pin);
bool hasBleEvent = false;
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
  Oled.drawString(0, 0, "Initializing BLE");
  Oled.display();

  Serial.println("Starting BLE");
  BLEinit(BLE_DEVICE_NAME, &hasBleEvent);
  BLEsetupAd();
  BLEstartAd();
  // delay(10000);
  // ScanWirelessNodes();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (hasBleEvent)
    switch (BLEreadEvent())
    {
    case ble_events::WIFI_START_SCAN:
      WiFiScanNodes();
      break;

    case ble_events::WIFI_CONNECTION_CHANGED:
      WiFiConnect(BLEgetSSID(), BLEgetPassword());
    default:
      break;
    }
}
