#include <Arduino.h>
#include <BLEuuids.h>
#include <NimBLEDevice.h>
#include <enums.h>
#include <StoreHub.h>

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
public:
    CharacteristicCallbacks(void (*event_queue_method)(const CustomEvents &), bool verbose = false);

private:
    const char *returnCodeToString(int rc);
    const char *returnStatusToString(Status status);
    void (*queueEvent)(const CustomEvents &);
    bool isVerbose;

    void onRead(NimBLECharacteristic *pCharacteristic);

    void onWrite(NimBLECharacteristic *pCharacteristic);

    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic *pCharacteristic);

    //  The status returned in status is defined in NimBLECharacteristic.h.
    //  The value returned in code is the NimBLE host return code.

    void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code);

    void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue);
};
