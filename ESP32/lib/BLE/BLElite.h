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
#include <ConnectionStatus.h>

class BLElite
{
public:
    void Initialize(std::string deviceName, void (*event_queue_method)(const CustomEvents &), bool verbose = false);
    void Setup();
    void StartAdvertise();
    void StopAdvertise();
    void SetGoogleHomeName(std::string value);
    void UpdateConnectionStatus(ConnectionStatus status);
    void UpdateSensorValues(float temperature, float humidity, float pressure, float airQuality);
    void SetSSID(std::string value);
    void SetSSIDs(std::string SSIDs);
    void StoreWiFiAuthentication();
    void StoreAzureAuthentication();

private:
    bool IsAdvertising;
    bool IsConnected;
    bool IsVerbose;
    void (*queueEvent)(const CustomEvents &);
    NimBLEServer *pServer;
    std::string getSSIDs();
    std::string getSSID();
    std::string getPassword();
    std::string getAzureAuthentication();
    void addCharacteristic(BLEService *pService, int uuid, uint32_t properties, std::string value, int descriptorUuid = -1, std::string descriptorValue = "");
    std::string getCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid);
    void setCharacteristicValue(BLEUUID serviceUuid, BLEUUID charateristicsUuid, std::string value);
    std::string convertToString(float value);
    bool hasNotifier(BLEUUID uuid);
};
