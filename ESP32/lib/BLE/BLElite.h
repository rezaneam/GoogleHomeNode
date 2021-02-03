#include <Arduino.h>
#include <StoreHub.h>
#include <stdio.h>
#include <string.h>
#include <NimBLEDevice.h>
#include <sstream>
#include <enums.h>
#include <BLEuuids.h>
#include <ServerCallbacks.h>
#include <CharacteristicCallbacks.h>

class BLElite
{
public:
    void Initialize(std::string deviceName, void (*event_queue_method)(CustomEvents), bool verbose = false);
    void Setup();
    void StartAdvertise();
    void StopAdvertise();
    void SetSSID(std::string value);
    void SetGoogleHomeName(std::string value);
    void UpdateConnectionStatus(bool isWiFiConnected, bool isGoogleHomeConnected, bool isAzureConnected);
    void UpdateSensorValues(float temperature, float humidity, float pressure);
    std::string getSSID();
    void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value);
    void SetSSIDs(std::string SSIDs);
    void addCharacteristic(BLEService *pService, int uuid, uint32_t properties, std::string value, int descriptorUuid = -1, std::string descriptorValue = "");
    std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid);
    std::string getSSIDs();
    std::string getPassword();

private:
    bool IsAdvertising;
    bool IsConnected;
    bool IsVerbose;
    void (*queueEvent)(CustomEvents);
    NimBLEServer *pServer;
    std::string convertToString(float value);
    bool hasNotifier(BLEUUID uuid);
};
