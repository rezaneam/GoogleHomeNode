#include <ble.h>

String message;
BLEServer *pServer;
ble_events event;
bool *phasEvent;
bool isConnected = false;
bool isAdvertising = false;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onStopAdvertise()
{
    portENTER_CRITICAL_ISR(&timerMux);
    if (!isConnected)
    {
        BLEstopAd();
        timerStop(timer);
        event = ble_events::BLE_STOPPED;
        *phasEvent = true;
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

void initTimer()
{
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onStopAdvertise, true);
    timerAlarmWrite(timer, 1000000 * BLE_ADVERTISE_TIMEOUT, true);
    timerAlarmEnable(timer);
    timerStop(timer);
}

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        isConnected = true;
        event = ble_events::BLE_CONNECTED;
        *phasEvent = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        isConnected = false;
        event = ble_events::BLE_DISCONNECT;
        *phasEvent = true;
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic){};

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        BLEUUID uuid = pCharacteristic->getUUID();
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING)))
        {
            std::string val = pCharacteristic->getValue();
            if (val == "1")
            {
                *phasEvent = true;
            }
            return;
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS)))
        {
            std::string val = pCharacteristic->getValue();
            event = ble_events::WIFI_CONNECTION_CHANGED;
            *phasEvent = true;
            return;
        }
    }
};

void BLEinit(std::string deviceName, bool *hasEvent)
{
    phasEvent = hasEvent;
    BLEDevice::init(deviceName);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // System ID  - Read Only
    BLEService *pDeviceInfoService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_DEVICE_INFORMATION));
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_SYSTEM_ID), BLECharacteristic::PROPERTY_READ, BLE_SYSTEM_ID);
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_MANUFACTURER), BLECharacteristic::PROPERTY_READ, BLE_MANUFACTURER);
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_MODEL_NUMBER), BLECharacteristic::PROPERTY_READ, BLE_MODEL_NUMBER);
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_FIRMWARE_REVISION), BLECharacteristic::PROPERTY_READ, BLE_FIRMWARE_REVISION);
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_HARDWARE_REVISION), BLECharacteristic::PROPERTY_READ, BLE_HARDWARE_REVISION);
    addCharacteristic(pDeviceInfoService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_SOFTWARE_REVISION), BLECharacteristic::PROPERTY_READ, BLE_SOFTWARE_REVISION);

    // Battery level - Read & Notify
    BLEService *pBatteryService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_BATTERY));
    addCharacteristic(pBatteryService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_BATTERY_LEVEL), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, "100%");

    // BME280 - Read & Notify
    BLEService *pSensorService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING));
    addCharacteristic(pSensorService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_PRESSURE), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, "N.A.");
    addCharacteristic(pSensorService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_TEMPERATURE), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, "N.A.");
    addCharacteristic(pSensorService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_HUMIDITY), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, "N.A.");

    // WiFi Configuration - Read, Notify, Write
    BLEService *pAutomationService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA));
    addCharacteristic(pAutomationService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, "");
    addCharacteristic(pAutomationService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE, BLE_WIFI_SCANNING_DEACTIVE);
    addCharacteristic(pAutomationService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE, "");
    addCharacteristic(pAutomationService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS), BLECharacteristic::PROPERTY_WRITE, "");
    addCharacteristic(pAutomationService, BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_CONNECTION_STAT), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY, BLE_WIFI_NOT_CONNECTED);

    pDeviceInfoService->start();
    pBatteryService->start();
    pSensorService->start();
    pAutomationService->start();

    initTimer();
}

void BLEsetupAd()
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_DEVICE_INFORMATION));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_BATTERY));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA));
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
}

void BLEstartAd()
{
    if (isAdvertising)
        return;
    isAdvertising = true;
    BLEDevice::startAdvertising();
    Serial.println("BLE starts advertising");
    timerStart(timer);
    event = ble_events::BLE_STARTED;
    *phasEvent = true;
}

void BLEstopAd()
{
    BLEDevice::getAdvertising()->stop();
    isAdvertising = false;
    Serial.println("BLE stopped advertising.");
}

void addCharacteristic(BLEService *pService, BLEUUID uuid, uint32_t properties, std::string value)
{
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(uuid, properties);

    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pCharacteristic->setValue(value);
}

void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value)
{
    pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->setValue(value);
    pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->notify();
}

void BLEsetSSIDs(std::string SSIDs)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES), SSIDs);
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING), BLE_WIFI_SCANNING_DEACTIVE);
}

void BLEwirelessConnectionChanged(std::string status)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_CONNECTION_STAT), status);
}

std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid)
{
    return pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->getValue();
}

ble_events BLEreadEvent()
{
    *phasEvent = false;
    return event;
}

std::string BLEgetSSIDs()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES));
}

std::string BLEgetSSID()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID));
}

std::string BLEgetPassword()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS));
}