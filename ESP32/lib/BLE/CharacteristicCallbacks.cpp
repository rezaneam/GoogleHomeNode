#include <CharacteristicCallbacks.h>
CharacteristicCallbacks::CharacteristicCallbacks(void (*event_queue_method)(CustomEvents), bool verbose)
{
    queueEvent = event_queue_method;
    isVerbose = verbose;
}

void CharacteristicCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
}

void CharacteristicCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
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
            queueEvent(CustomEvents::EVENT_WIFI_START_SCAN);
        }
        return;
    }
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_WIFI_PASS)))
    {
        queueEvent(CustomEvents::EVENT_WIFI_PASS_RECEIVED);
        return;
    }
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_GOOGLE_HOME_NAME)))
    {
        WriteFlashHomeName(pCharacteristic->getValue());
        queueEvent(CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT);
        return;
    }
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_AZURE_IOT_HUB_CONN)))
    {
        queueEvent(CustomEvents::EVENT_WIFI_AZURE_KEY_RECEIVED);
        return;
    }
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_CONNECTION_STAT)))
    {
        std::string val = pCharacteristic->getValue();
        if (val[0] == '1') //
            queueEvent(CustomEvents::EVENT_WIFI_TRY_CONNECT);

        else if (val[1] == '1') //
            queueEvent(CustomEvents::EVENT_GOOGLE_HOME_TRY_CONNECT);

        else if (val[2] == '1') //
            queueEvent(CustomEvents::EVENT_AZURE_IOT_HUB_TRY_CONNECT);
        return;
    }
    if (uuid.equals(BLEUUID((uint16_t)CHARACTERISTIC_UUID_RESET_CONFIG)))
    {
        std::string val = pCharacteristic->getValue();
        if (val == "1")
            queueEvent(CustomEvents::EVENT_RESTART);
        else if (val == "2")
            queueEvent(CustomEvents::EVENT_FACTORY_RESET);

        else if (val == "3")
            queueEvent(CustomEvents::EVENT_FACTORY_RESET_SAFE);
    }
}

void CharacteristicCallbacks::onNotify(NimBLECharacteristic *pCharacteristic)
{
    Serial.println("Sending notification to clients");
}

void CharacteristicCallbacks::onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
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

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
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