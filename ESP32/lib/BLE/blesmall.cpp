#include <blesmall.h>

NimBLEServer *pServer;
CustomEvents *pEvent;
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
        *pEvent = CustomEvents::EVENT_BLE_STOPPED;
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

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer)
    {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();

        isConnected = true;
        *pEvent = CustomEvents::EVENT_BLE_CONNECTED;
    };
    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
    {
        Serial.println("Client connected");
        Serial.print("Client address: ");
        Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);

        isConnected = true;
        *pEvent = CustomEvents::EVENT_BLE_CONNECTED;
    };
    void onDisconnect(NimBLEServer *pServer)
    {
        Serial.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();

        isConnected = false;
        *pEvent = CustomEvents::EVENT_BLE_DISCONNECT;
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest()
    {
        Serial.println("Server Passkey Request");
        /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key)
    {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc *desc)
    {
        /** Check that encryption was successful, if not we disconnect the client */
        if (!desc->sec_state.encrypted)
        {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            Serial.println("Encrypt connection failed - disconnecting client");
            return;
        }
        Serial.println("Starting BLE work!");
    };
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
    void onRead(NimBLECharacteristic *pCharacteristic)
    {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
        NimBLEUUID uuid = pCharacteristic->getUUID();
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING)))
        {
            std::string val = pCharacteristic->getValue();
            if (val == "1")
            {
                *pEvent = CustomEvents::EVENT_WIFI_START_SCAN;
            }
            return;
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS)))
        {
            WriteFlashWiFi(BLEgetSSID(), pCharacteristic->getValue());
            setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), uuid, BLE_WIFI_PASS_WRITE_ONLY);
            return;
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_GOOGLE_HOME_NAME)))
        {
            WriteFlashHomeName(pCharacteristic->getValue());
            *pEvent = CustomEvents::EVENT_GOOGLE_HOME_NAME;
            return;
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_AZURE_IOT_HUB_CONN)))
        {
            WriteAzureIoTHub(pCharacteristic->getValue());
            setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), uuid, BLE_WIFI_PASS_WRITE_ONLY);
            *pEvent = CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT;
            return;
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_CONNECTION_STAT)))
        {
            std::string val = pCharacteristic->getValue();
            if (val[0] = '1') //
            {
                *pEvent = CustomEvents::EVENT_WIFI_TRY_CONNECT;
                return;
            }
            if (val[1] = '1') //
            {
                *pEvent = CustomEvents::EVENT_GOOGLE_TYPE_CONNECT;
                return;
            }
            if (val[2] = '1') //
            {
                *pEvent = CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT;
                return;
            }
        }
        if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_RESET_CONFIG)))
        {
            std::string val = pCharacteristic->getValue();
            if (val == "1")
                *pEvent = CustomEvents::EVENT_RESTART;

            else if (val == "2")
                *pEvent = CustomEvents::EVENT_FACTORY_RESET;

            else if (val == "3")
                *pEvent = CustomEvents::EVENT_FACTORY_RESET_SAFE;
        }
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic *pCharacteristic)
    {
        Serial.println("Sending notification to clients");
    };

    //  The status returned in status is defined in NimBLECharacteristic.h.
    //  The value returned in code is the NimBLE host return code.

    void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
    {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        str += ", ";
        str += String(pCharacteristic->getUUID().toString().c_str());
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
    {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if (subValue == 0)
        {
            str += " Unsubscribed to ";
        }
        else if (subValue == 1)
        {
            str += " Subscribed to notfications for ";
        }
        else if (subValue == 2)
        {
            str += " Subscribed to indications for ";
        }
        else if (subValue == 3)
        {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

void BLEinit(std::string deviceName, CustomEvents *event)
{
    pEvent = event;
    Serial.println("Free heap is " + String(ESP.getFreeHeap()));
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
    Serial.println("Free heap is " + String(ESP.getFreeHeap()));

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

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
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_CONNECTION_STAT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE, BLE_WIFI_NOT_CONNECTED, DESCRIPTOR_UUID_WIFI_CONN, DESCRIPTOR_VAL_WIFI_CONN);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_GOOGLE_HOME_NAME, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE, "", DESCRIPTOR_UUID_GLHM_NAME, DESCRIPTOR_VAL_GLHM_NAME);
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_AZURE_IOT_HUB_CONN, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, "");
    addCharacteristic(pAutomationService, CHARACTERISTIC_UUID_DEVICE_LOCATION, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, "");

    pDeviceInfoService->start();
    pBatteryService->start();
    pSensorService->start();
    pAutomationService->start();

    initTimer();
}

void BLEsetupAd()
{
    NimBLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_DEVICE_INFORMATION));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_BATTERY));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING));
    pAdvertising->addServiceUUID(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA));
    pAdvertising->setScanResponse(true);
    //pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    //pAdvertising->setMinPreferred(0x12);
}

void BLEstartAd()
{
    if (isAdvertising)
        return;
    isAdvertising = true;
    // NimBLEDevice::startAdvertising();
    Serial.println("BLE starts advertising");
    timerStart(timer);
    *pEvent = CustomEvents::EVENT_BLE_STARTED;
}

void BLEstopAd()
{
    // NimBLEDevice::stopAdvertising();
    isAdvertising = false;
    Serial.println("BLE stopped advertising.");
}

void addCharacteristic(BLEService *pService, int uuid, uint32_t properties, std::string value, int descriptorUuid, std::string descriptorValue)
{
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(BLEUUID((uint16_t)uuid), properties);

    pCharacteristic->setCallbacks(new CharacteristicCallbacks());
    pCharacteristic->setValue(value);

    if (descriptorUuid == -1)
        return;
    NimBLEDescriptor *pDescriptor = (NimBLE2904 *)pCharacteristic->createDescriptor(BLEUUID((uint16_t)descriptorUuid));
    pDescriptor->setValue(descriptorValue);
    NimBLE2904 *pDescriptor2904 = (NimBLE2904 *)pCharacteristic->createDescriptor("2904");
    pDescriptor2904->setFormat(NimBLE2904::FORMAT_UTF8);
}

void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value)
{
    pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->setValue(value);
    if (hasNotifier(charateristicsUuid))
        pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->notify();
}

void BLEsetSSIDs(std::string SSIDs)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES), SSIDs);
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SCANNING), BLE_WIFI_SCANNING_DEACTIVE);
}

void BLEwirelessConnectionChanged(std::string status)
{
    if (status == BLE_WIFI_CONNECTED)
        *pEvent = CustomEvents::EVENT_WIFI_CONNECTED;
    else
        *pEvent = CustomEvents::EVENT_WIFI_DISCONNECTED;
}

std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid)
{
    return pServer->getServiceByUUID(serviceUuid)->getCharacteristic(charateristicsUuid)->getValue();
}

std::string BLEgetSSIDs()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID_NAMES));
}

void BLEsetSSID(std::string value)
{
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID), value);
}

std::string BLEgetSSID()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_SSID));
}

std::string BLEgetPassword()
{
    return getCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS));
}

void UpdateSensorValues(float temperature, float humidity, float pressure)
{
    if (!isConnected)
        return;
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_TEMPERATURE), convertToString(temperature));
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_HUMIDITY), convertToString(humidity));
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_ENVIROMENTAL_SENSING), BLEUUID((uint16_t)CHARACTERISTIC_UUID_PRESSURE), convertToString(pressure));
}

std::string convertToString(float value)
{
    std::ostringstream sstream;
    sstream << value;
    return sstream.str();
}

bool hasNotifier(BLEUUID uuid)
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

void BLEupdateConnectionStatus(bool isWiFiConnected, bool isGoogleHomeConnected, bool isAzureConnected)
{
    std::string status;
    status.push_back(isWiFiConnected ? '2' : '0');
    status.push_back(isGoogleHomeConnected ? '2' : '0');
    status.push_back(isAzureConnected ? '2' : '0');
    setCharacteristicValue(BLEUUID((uint16_t)SERVICE_UUID_USER_DATA), BLEUUID((uint16_t)CHARACTERISTIC_UUID_CONNECTION_STAT), status);
}