#include <Arduino.h>
#include <BLEuuids.h>
#include <NimBLEDevice.h>
#include <enums.h>

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks : public NimBLEServerCallbacks
{
public:
    ServerCallbacks(void (*event_queue_method)(const CustomEvents &), bool *connectionStatus, bool verbose = false);

private:
    void (*queueEvent)(const CustomEvents &);
    bool isVerbose;
    bool *isConnected;

    void onConnect(NimBLEServer *pServer);

    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc);

    void onDisconnect(NimBLEServer *pServer);

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest();

    bool onConfirmPIN(uint32_t pass_key);

    void onAuthenticationComplete(ble_gap_conn_desc *desc);
};
