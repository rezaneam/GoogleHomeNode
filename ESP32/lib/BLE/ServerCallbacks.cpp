#include <ServerCallbacks.h>

ServerCallbacks::ServerCallbacks(void (*event_queue_method)(CustomEvents), bool *connectionStatus, bool verbose)
{
    queueEvent = event_queue_method;
    isConnected = connectionStatus;
    isVerbose = verbose;
}

void ServerCallbacks::onConnect(NimBLEServer *pServer)
{
    if (isVerbose)
        printf("Client connected.\r\n Multi-connect support: start advertising\r\n");
    NimBLEDevice::startAdvertising();

    *isConnected = true;
    queueEvent(CustomEvents::EVENT_BLE_CONNECTED);
};
/** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
void ServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
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

    *isConnected = true;
    queueEvent(CustomEvents::EVENT_BLE_CONNECTED);
};
void ServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
    Serial.println("Client DisConnected - start advertising");
    NimBLEDevice::startAdvertising();

    *isConnected = false;
    queueEvent(CustomEvents::EVENT_BLE_DISCONNECT);
};

/********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
uint32_t ServerCallbacks::onPassKeyRequest()
{
    Serial.println("Server Passkey Request");
    /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
    return 123456;
};

bool ServerCallbacks::onConfirmPIN(uint32_t pass_key)
{
    Serial.print("The passkey YES/NO number: ");
    Serial.println(pass_key);
    /** Return false if passkeys don't match. */
    return true;
};

void ServerCallbacks::onAuthenticationComplete(ble_gap_conn_desc *desc)
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
