#include <BLElite.h>

std::string BLElite::convertToString(float value)
{
    std::ostringstream sstream;
    sstream << value;
    return sstream.str();
}

bool BLElite::hasNotifier(BLEUUID uuid)
{
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_BATTERY_LEVEL)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_PRESSURE)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_TEMPERATURE)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_HUMIDITY)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_CONNECTION_STAT)))
        return true;
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_GOOGLE_HOME_NAME)))
        return true;
    return false;
}

void BLElite::Initialize(std::string deviceName, void (*event_queue_method)(CustomEvents), bool verbose)
{
    queueEvent = event_queue_method;
    IsVerbose = verbose;
    //serverCallback->Initialize(event_queue_method, &IsConnected, verbose);
    //characteristicCallback->Initialize(event_queue_method, verbose);

    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(event_queue_method, &IsConnected, verbose));

    // System ID  - Read Only
    NimBLEService *pDeviceInfoService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_DEVICE_INFORMATION));
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_SYSTEM_ID, NIMBLE_PROPERTY::READ, BLE_SYSTEM_ID);
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_MANUFACTURER, NIMBLE_PROPERTY::READ, BLE_MANUFACTURER);
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_MODEL_NUMBER, NIMBLE_PROPERTY::READ, BLE_MODEL_NUMBER);
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_FIRMWARE_REVISION, NIMBLE_PROPERTY::READ, BLE_FIRMWARE_REVISION);
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_HARDWARE_REVISION, NIMBLE_PROPERTY::READ, BLE_HARDWARE_REVISION);
    addCharacteristic(pDeviceInfoService, CHARACTERISTIC_UUID_SOFTWARE_REVISION, NIMBLE_PROPERTY::READ, BLE_SOFTWARE_REVISION);

    // Battery level - Read & Notify
    NimBLEService *pBatteryService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_BATTERY));
    addCharacteristic(pBatteryService, CHARACTERISTIC_UUID_BATTERY_LEVEL, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY, "100%", DESCRIPTOR_UUID_BATTERY, DESCRIPTOR_VAL_BATTERY);

    // BME280 - Read & Notify
    NimBLEService *pSensorService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING));
    addCharacteristic(pSensorService, CHARACTERISTIC_UUID_PRESSURE, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY, "N.A.", DESCRIPTOR_UUID_PRESSURE, DESCRIPTOR_VAL_PRESSURE);
    addCharacteristic(pSensorService, CHARACTERISTIC_UUID_TEMPERATURE, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY, "N.A.", DESCRIPTOR_UUID_TEMPERATURE, DESCRIPTOR_VAL_TEMPEATURE);
    addCharacteristic(pSensorService, CHARACTERISTIC_UUID_HUMIDITY, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY, "N.A.", DESCRIPTOR_UUID_HUMIDITY, DESCRIPTOR_VAL_HUMIDITY);

    // WiFi Configuration - Read, Notify, Write
    NimBLEService *pAutomationService = pServer->createService(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA));

    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_RESET_CONFIG, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ, "0");
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_WIFI_SSID_NAMES, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY, "");
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_WIFI_SCANNING, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE, BLE_WIFI_SCANNING_DEACTIVE, DESCRIPTOR_UUID_WIFI_SCAN, DESCRIPTOR_VAL_WIFI_SCAN);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_WIFI_SSID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, "");
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_WIFI_PASS, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, BLE_WIFI_PASS_WRITE_ONLY);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_CONNECTION_STAT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE, BLE_NOT_CONNECTED, DESCRIPTOR_UUID_WIFI_CONN, DESCRIPTOR_VAL_WIFI_CONN);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_GOOGLE_HOME_NAME, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE, "", DESCRIPTOR_UUID_GLHM_NAME, DESCRIPTOR_VAL_GLHM_NAME);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_AZURE_IOT_HUB_CONN, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, "");
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_DEVICE_LOCATION, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, "");

    pDeviceInfoService->start();
    pBatteryService->start();
    pSensorService->start();
    pAutomationService->start();
}

void BLElite::Setup()
{
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_DEVICE_INFORMATION));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_BATTERY));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA));
    pAdvertising->setScanResponse(true);
    //pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    //pAdvertising->setMinPreferred(0x12);
}

void BLElite::StartAdvertise()
{
    if (IsAdvertising)
        return;
    IsAdvertising = true;
    NimBLEDevice::startAdvertising();
    Serial.println("BLE starts advertising");
}

void BLElite::StopAdvertise()
{
    if (!IsAdvertising)
        return;
    IsAdvertising = false;
    NimBLEDevice::stopAdvertising();
    Serial.println("BLE stopped advertising");
}

void BLElite::addCharacteristic(BLEService *pService, int uuid, uint32_t properties, std::string value, int descriptorUuid, std::string descriptorValue)
{
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(BLEUUID((uint16_t)uuid), properties);

    pCharacteristic->setCallbacks(new CharacteristicCallbacks(queueEvent, IsVerbose));
    pCharacteristic->setValue(value);

    if (descriptorUuid == -1)
        return;
    NimBLEDescriptor *pDescriptor = (NimBLE2904 *)pCharacteristic->createDescriptor(BLEUUID((uint16_t)descriptorUuid));
    pDescriptor->setValue(descriptorValue);
    NimBLE2904 *pDescriptor2904 = (NimBLE2904 *)pCharacteristic->createDescriptor("2904");
    pDescriptor2904->setFormat(NimBLE2904::FORMAT_UTF8);
}

void BLElite::setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value)
{
    pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->setValue(value);
    if (hasNotifier(charateristicsUuid))
        pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->notify();
}

void BLElite::SetSSIDs(std::string SSIDs)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES), SSIDs);
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING), BLE_WIFI_SCANNING_DEACTIVE);
}

std::string BLElite::getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid)
{
    return pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->getValue();
}

std::string BLElite::getSSIDs()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES));
}

void BLElite::SetSSID(std::string value)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID), value);
}

void BLElite::SetGoogleHomeName(std::string value)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_GOOGLE_HOME_NAME), value);
}

std::string BLElite::getSSID()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID));
}

std::string BLElite::getPassword()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS));
}

void BLElite::UpdateSensorValues(float temperature, float humidity, float pressure)
{
    if (!IsConnected)
        return;
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_TEMPERATURE), convertToString(temperature));
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_HUMIDITY), convertToString(humidity));
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_PRESSURE), convertToString(pressure));
}

void BLElite::UpdateConnectionStatus(bool isWiFiConnected, bool isGoogleHomeConnected, bool isAzureConnected)
{
    std::string status;
    status.push_back(isWiFiConnected ? '2' : '0');
    status.push_back(isGoogleHomeConnected ? '2' : '0');
    status.push_back(isAzureConnected ? '2' : '0');
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_CONNECTION_STAT), status);
}
