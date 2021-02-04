#include <CharacteristicCallbacks.h>
CharacteristicCallbacks::CharacteristicCallbacks(void (*event_queue_method)(CustomEvents), bool verbose)
{
    queueEvent = event_queue_method;
    isVerbose = verbose;
}

void CharacteristicCallbacks::onRead(NimBLECharacteristic *pCharacteristic)
{
    if (isVerbose)
        printf("%s : onRead(), value: %s\r\n", pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getValue().c_str());
}

void CharacteristicCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    if (isVerbose)
        printf("%s : onWrite(), value: %s\r\n", pCharacteristic->getUUID().toString().c_str(), pCharacteristic->getValue().c_str());
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
    if (isVerbose)
        printf("Sending notification to clients\r\n");
}

void CharacteristicCallbacks::onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
{
    if (isVerbose)
        printf("%s Notification/Indication >> status: %s, return: %s\r\n",
               pCharacteristic->getUUID().toString().c_str(), returnStatusToString(status), returnCodeToString(code));
};

void CharacteristicCallbacks::onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
{
    if (!isVerbose)
        return;
    if (subValue == 0)
        printf("Client ID: %d Address: %s %s %s\r\n",
               desc->conn_handle, std::string(NimBLEAddress(desc->peer_ota_addr)).c_str(),
               "Unsubscribed to", std::string(pCharacteristic->getUUID()).c_str());
    else if (subValue == 1)
        printf("Client ID: %d Address: %s %s %s\r\n",
               desc->conn_handle, std::string(NimBLEAddress(desc->peer_ota_addr)).c_str(),
               "Subscribed to notfications for", std::string(pCharacteristic->getUUID()).c_str());
    else if (subValue == 2)
        printf("Client ID: %d Address: %s %s %s\r\n",
               desc->conn_handle, std::string(NimBLEAddress(desc->peer_ota_addr)).c_str(),
               "Subscribed to indications for", std::string(pCharacteristic->getUUID()).c_str());
    else if (subValue == 3)
        printf("Client ID: %d Address: %s %s %s\r\n",
               desc->conn_handle, std::string(NimBLEAddress(desc->peer_ota_addr)).c_str(),
               "Subscribed to notifications and indications for", std::string(pCharacteristic->getUUID()).c_str());
};

/**
 * @brief Converts a return code from the NimBLE stack to a text string.
 * @param [in] rc The return code to convert.
 * @return A string representation of the return code.
 */
const char *CharacteristicCallbacks::returnCodeToString(int rc)
{
    switch (rc)
    {
    case 0:
        return "SUCCESS";
    case BLE_HS_EAGAIN:
        return "Temporary failure; try again.";
    case BLE_HS_EALREADY:
        return "Operation already in progress or completed.";
    case BLE_HS_EINVAL:
        return "One or more arguments are invalid.";
    case BLE_HS_EMSGSIZE:
        return "The provided buffer is too small.";
    case BLE_HS_ENOENT:
        return "No entry matching the specified criteria.";
    case BLE_HS_ENOMEM:
        return "Operation failed due to resource exhaustion.";
    case BLE_HS_ENOTCONN:
        return "No open connection with the specified handle.";
    case BLE_HS_ENOTSUP:
        return "Operation disabled at compile time.";
    case BLE_HS_EAPP:
        return "Application callback behaved unexpectedly.";
    case BLE_HS_EBADDATA:
        return "Command from peer is invalid.";
    case BLE_HS_EOS:
        return "Mynewt OS error.";
    case BLE_HS_ECONTROLLER:
        return "Event from controller is invalid.";
    case BLE_HS_ETIMEOUT:
        return "Operation timed out.";
    case BLE_HS_EDONE:
        return "Operation completed successfully.";
    case BLE_HS_EBUSY:
        return "Operation cannot be performed until procedure completes.";
    case BLE_HS_EREJECT:
        return "Peer rejected a connection parameter update request.";
    case BLE_HS_EUNKNOWN:
        return "Unexpected failure; catch all.";
    case BLE_HS_EROLE:
        return "Operation requires different role (e.g., central vs. peripheral).";
    case BLE_HS_ETIMEOUT_HCI:
        return "HCI request timed out; controller unresponsive.";
    case BLE_HS_ENOMEM_EVT:
        return "Controller failed to send event due to memory exhaustion (combined host-controller only).";
    case BLE_HS_ENOADDR:
        return "Operation requires an identity address but none configured.";
    case BLE_HS_ENOTSYNCED:
        return "Attempt to use the host before it is synced with controller.";
    case BLE_HS_EAUTHEN:
        return "Insufficient authentication.";
    case BLE_HS_EAUTHOR:
        return "Insufficient authorization.";
    case BLE_HS_EENCRYPT:
        return "Insufficient encryption level.";
    case BLE_HS_EENCRYPT_KEY_SZ:
        return "Insufficient key size.";
    case BLE_HS_ESTORE_CAP:
        return "Storage at capacity.";
    case BLE_HS_ESTORE_FAIL:
        return "Storage IO error.";
    case (0x0100 + BLE_ATT_ERR_INVALID_HANDLE):
        return "The attribute handle given was not valid on this server.";
    case (0x0100 + BLE_ATT_ERR_READ_NOT_PERMITTED):
        return "The attribute cannot be read.";
    case (0x0100 + BLE_ATT_ERR_WRITE_NOT_PERMITTED):
        return "The attribute cannot be written.";
    case (0x0100 + BLE_ATT_ERR_INVALID_PDU):
        return "The attribute PDU was invalid.";
    case (0x0100 + BLE_ATT_ERR_INSUFFICIENT_AUTHEN):
        return "The attribute requires authentication before it can be read or written.";
    case (0x0100 + BLE_ATT_ERR_REQ_NOT_SUPPORTED):
        return "Attribute server does not support the request received from the client.";
    case (0x0100 + BLE_ATT_ERR_INVALID_OFFSET):
        return "Offset specified was past the end of the attribute.";
    case (0x0100 + BLE_ATT_ERR_INSUFFICIENT_AUTHOR):
        return "The attribute requires authorization before it can be read or written.";
    case (0x0100 + BLE_ATT_ERR_PREPARE_QUEUE_FULL):
        return "Too many prepare writes have been queued.";
    case (0x0100 + BLE_ATT_ERR_ATTR_NOT_FOUND):
        return "No attribute found within the given attribute handle range.";
    case (0x0100 + BLE_ATT_ERR_ATTR_NOT_LONG):
        return "The attribute cannot be read or written using the Read Blob Request.";
    case (0x0100 + BLE_ATT_ERR_INSUFFICIENT_KEY_SZ):
        return "The Encryption Key Size used for encrypting this link is insufficient.";
    case (0x0100 + BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN):
        return "The attribute value length is invalid for the operation.";
    case (0x0100 + BLE_ATT_ERR_UNLIKELY):
        return "The attribute request has encountered an error that was unlikely, could not be completed as requested.";
    case (0x0100 + BLE_ATT_ERR_INSUFFICIENT_ENC):
        return "The attribute requires encryption before it can be read or written.";
    case (0x0100 + BLE_ATT_ERR_UNSUPPORTED_GROUP):
        return "The attribute type is not a supported grouping attribute as defined by a higher layer specification.";
    case (0x0100 + BLE_ATT_ERR_INSUFFICIENT_RES):
        return "Insufficient Resources to complete the request.";
    case (0x0200 + BLE_ERR_UNKNOWN_HCI_CMD):
        return "Unknown HCI Command";
    case (0x0200 + BLE_ERR_UNK_CONN_ID):
        return "Unknown Connection Identifier";
    case (0x0200 + BLE_ERR_HW_FAIL):
        return "Hardware Failure";
    case (0x0200 + BLE_ERR_PAGE_TMO):
        return "Page Timeout";
    case (0x0200 + BLE_ERR_AUTH_FAIL):
        return "Authentication Failure";
    case (0x0200 + BLE_ERR_PINKEY_MISSING):
        return "PIN or Key Missing";
    case (0x0200 + BLE_ERR_MEM_CAPACITY):
        return "Memory Capacity Exceeded";
    case (0x0200 + BLE_ERR_CONN_SPVN_TMO):
        return "Connection Timeout";
    case (0x0200 + BLE_ERR_CONN_LIMIT):
        return "Connection Limit Exceeded";
    case (0x0200 + BLE_ERR_SYNCH_CONN_LIMIT):
        return "Synchronous Connection Limit To A Device Exceeded";
    case (0x0200 + BLE_ERR_ACL_CONN_EXISTS):
        return "ACL Connection Already Exists";
    case (0x0200 + BLE_ERR_CMD_DISALLOWED):
        return "Command Disallowed";
    case (0x0200 + BLE_ERR_CONN_REJ_RESOURCES):
        return "Connection Rejected due to Limited Resources";
    case (0x0200 + BLE_ERR_CONN_REJ_SECURITY):
        return "Connection Rejected Due To Security Reasons";
    case (0x0200 + BLE_ERR_CONN_REJ_BD_ADDR):
        return "Connection Rejected due to Unacceptable BD_ADDR";
    case (0x0200 + BLE_ERR_CONN_ACCEPT_TMO):
        return "Connection Accept Timeout Exceeded";
    case (0x0200 + BLE_ERR_UNSUPPORTED):
        return "Unsupported Feature or Parameter Value";
    case (0x0200 + BLE_ERR_INV_HCI_CMD_PARMS):
        return "Invalid HCI Command Parameters";
    case (0x0200 + BLE_ERR_REM_USER_CONN_TERM):
        return "Remote User Terminated Connection";
    case (0x0200 + BLE_ERR_RD_CONN_TERM_RESRCS):
        return "Remote Device Terminated Connection due to Low Resources";
    case (0x0200 + BLE_ERR_RD_CONN_TERM_PWROFF):
        return "Remote Device Terminated Connection due to Power Off";
    case (0x0200 + BLE_ERR_CONN_TERM_LOCAL):
        return "Connection Terminated By Local Host";
    case (0x0200 + BLE_ERR_REPEATED_ATTEMPTS):
        return "Repeated Attempts";
    case (0x0200 + BLE_ERR_NO_PAIRING):
        return "Pairing Not Allowed";
    case (0x0200 + BLE_ERR_UNK_LMP):
        return "Unknown LMP PDU";
    case (0x0200 + BLE_ERR_UNSUPP_REM_FEATURE):
        return "Unsupported Remote Feature / Unsupported LMP Feature";
    case (0x0200 + BLE_ERR_SCO_OFFSET):
        return "SCO Offset Rejected";
    case (0x0200 + BLE_ERR_SCO_ITVL):
        return "SCO Interval Rejected";
    case (0x0200 + BLE_ERR_SCO_AIR_MODE):
        return "SCO Air Mode Rejected";
    case (0x0200 + BLE_ERR_INV_LMP_LL_PARM):
        return "Invalid LMP Parameters / Invalid LL Parameters";
    case (0x0200 + BLE_ERR_UNSPECIFIED):
        return "Unspecified Error";
    case (0x0200 + BLE_ERR_UNSUPP_LMP_LL_PARM):
        return "Unsupported LMP Parameter Value / Unsupported LL Parameter Value";
    case (0x0200 + BLE_ERR_NO_ROLE_CHANGE):
        return "Role Change Not Allowed";
    case (0x0200 + BLE_ERR_LMP_LL_RSP_TMO):
        return "LMP Response Timeout / LL Response Timeout";
    case (0x0200 + BLE_ERR_LMP_COLLISION):
        return "LMP Error Transaction Collision";
    case (0x0200 + BLE_ERR_LMP_PDU):
        return "LMP PDU Not Allowed";
    case (0x0200 + BLE_ERR_ENCRYPTION_MODE):
        return "Encryption Mode Not Acceptable";
    case (0x0200 + BLE_ERR_LINK_KEY_CHANGE):
        return "Link Key cannot be Changed";
    case (0x0200 + BLE_ERR_UNSUPP_QOS):
        return "Requested QoS Not Supported";
    case (0x0200 + BLE_ERR_INSTANT_PASSED):
        return "Instant Passed";
    case (0x0200 + BLE_ERR_UNIT_KEY_PAIRING):
        return "Pairing With Unit Key Not Supported";
    case (0x0200 + BLE_ERR_DIFF_TRANS_COLL):
        return "Different Transaction Collision";
    case (0x0200 + BLE_ERR_QOS_PARM):
        return "QoS Unacceptable Parameter";
    case (0x0200 + BLE_ERR_QOS_REJECTED):
        return "QoS Rejected";
    case (0x0200 + BLE_ERR_CHAN_CLASS):
        return "Channel Classification Not Supported";
    case (0x0200 + BLE_ERR_INSUFFICIENT_SEC):
        return "Insufficient Security";
    case (0x0200 + BLE_ERR_PARM_OUT_OF_RANGE):
        return "Parameter Out Of Mandatory Range";
    case (0x0200 + BLE_ERR_PENDING_ROLE_SW):
        return "Role Switch Pending";
    case (0x0200 + BLE_ERR_RESERVED_SLOT):
        return "Reserved Slot Violation";
    case (0x0200 + BLE_ERR_ROLE_SW_FAIL):
        return "Role Switch Failed";
    case (0x0200 + BLE_ERR_INQ_RSP_TOO_BIG):
        return "Extended Inquiry Response Too Large";
    case (0x0200 + BLE_ERR_SEC_SIMPLE_PAIR):
        return "Secure Simple Pairing Not Supported By Host";
    case (0x0200 + BLE_ERR_HOST_BUSY_PAIR):
        return "Host Busy - Pairing";
    case (0x0200 + BLE_ERR_CONN_REJ_CHANNEL):
        return "Connection Rejected, No Suitable Channel Found";
    case (0x0200 + BLE_ERR_CTLR_BUSY):
        return "Controller Busy";
    case (0x0200 + BLE_ERR_CONN_PARMS):
        return "Unacceptable Connection Parameters";
    case (0x0200 + BLE_ERR_DIR_ADV_TMO):
        return "Directed Advertising Timeout";
    case (0x0200 + BLE_ERR_CONN_TERM_MIC):
        return "Connection Terminated due to MIC Failure";
    case (0x0200 + BLE_ERR_CONN_ESTABLISHMENT):
        return "Connection Failed to be Established";
    case (0x0200 + BLE_ERR_MAC_CONN_FAIL):
        return "MAC Connection Failed";
    case (0x0200 + BLE_ERR_COARSE_CLK_ADJ):
        return "Coarse Clock Adjustment Rejected";
    case (0x0300 + BLE_L2CAP_SIG_ERR_CMD_NOT_UNDERSTOOD):
        return "Invalid or unsupported incoming L2CAP sig command.";
    case (0x0300 + BLE_L2CAP_SIG_ERR_MTU_EXCEEDED):
        return "Incoming packet too large.";
    case (0x0300 + BLE_L2CAP_SIG_ERR_INVALID_CID):
        return "No channel with specified ID.";
    case (0x0400 + BLE_SM_ERR_PASSKEY):
        return "The user input of passkey failed, for example, the user cancelled the operation.";
    case (0x0400 + BLE_SM_ERR_OOB):
        return "The OOB data is not available.";
    case (0x0400 + BLE_SM_ERR_AUTHREQ):
        return "The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices.";
    case (0x0400 + BLE_SM_ERR_CONFIRM_MISMATCH):
        return "The confirm value does not match the calculated compare value.";
    case (0x0400 + BLE_SM_ERR_PAIR_NOT_SUPP):
        return "Pairing is not supported by the device.";
    case (0x0400 + BLE_SM_ERR_ENC_KEY_SZ):
        return "The resultant encryption key size is insufficient for the security requirements of this device.";
    case (0x0400 + BLE_SM_ERR_CMD_NOT_SUPP):
        return "The SMP command received is not supported on this device.";
    case (0x0400 + BLE_SM_ERR_UNSPECIFIED):
        return "Pairing failed due to an unspecified reason.";
    case (0x0400 + BLE_SM_ERR_REPEATED):
        return "Pairing or authentication procedure disallowed, too little time has elapsed since last pairing request or security request.";
    case (0x0400 + BLE_SM_ERR_INVAL):
        return "Command length is invalid or that a parameter is outside of the specified range.";
    case (0x0400 + BLE_SM_ERR_DHKEY):
        return "DHKey Check value received doesn't match the one calculated by the local device.";
    case (0x0400 + BLE_SM_ERR_NUMCMP):
        return "Confirm values in the numeric comparison protocol do not match.";
    case (0x0400 + BLE_SM_ERR_ALREADY):
        return "Pairing over the LE transport failed - Pairing Request sent over the BR/EDR transport in process.";
    case (0x0400 + BLE_SM_ERR_CROSS_TRANS):
        return "BR/EDR Link Key generated on the BR/EDR transport cannot be used to derive and distribute keys for the LE transport.";
    case (0x0500 + BLE_SM_ERR_PASSKEY):
        return "The user input of passkey failed or the user cancelled the operation.";
    case (0x0500 + BLE_SM_ERR_OOB):
        return "The OOB data is not available.";
    case (0x0500 + BLE_SM_ERR_AUTHREQ):
        return "The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices.";
    case (0x0500 + BLE_SM_ERR_CONFIRM_MISMATCH):
        return "The confirm value does not match the calculated compare value.";
    case (0x0500 + BLE_SM_ERR_PAIR_NOT_SUPP):
        return "Pairing is not supported by the device.";
    case (0x0500 + BLE_SM_ERR_ENC_KEY_SZ):
        return "The resultant encryption key size is insufficient for the security requirements of this device.";
    case (0x0500 + BLE_SM_ERR_CMD_NOT_SUPP):
        return "The SMP command received is not supported on this device.";
    case (0x0500 + BLE_SM_ERR_UNSPECIFIED):
        return "Pairing failed due to an unspecified reason.";
    case (0x0500 + BLE_SM_ERR_REPEATED):
        return "Pairing or authentication procedure is disallowed because too little time has elapsed since last pairing request or security request.";
    case (0x0500 + BLE_SM_ERR_INVAL):
        return "Command length is invalid or a parameter is outside of the specified range.";
    case (0x0500 + BLE_SM_ERR_DHKEY):
        return "Indicates to the remote device that the DHKey Check value received doesn’t match the one calculated by the local device.";
    case (0x0500 + BLE_SM_ERR_NUMCMP):
        return "Confirm values in the numeric comparison protocol do not match.";
    case (0x0500 + BLE_SM_ERR_ALREADY):
        return "Pairing over the LE transport failed - Pairing Request sent over the BR/EDR transport in process.";
    case (0x0500 + BLE_SM_ERR_CROSS_TRANS):
        return "BR/EDR Link Key generated on the BR/EDR transport cannot be used to derive and distribute keys for the LE transport.";
    default:
        return "Unknown";
    }
}

const char *CharacteristicCallbacks::returnStatusToString(Status status)
{
    switch (status)
    {
    case SUCCESS_INDICATE:
        return "Success Indicate";

    case SUCCESS_NOTIFY:
        return "Success Notify";

    case ERROR_INDICATE_DISABLED:
        return "Error Indicate Disabled";

    case ERROR_NOTIFY_DISABLED:
        return "Error Notify Disabled";

    case ERROR_GATT:
        return "Error GATT";

    case ERROR_NO_CLIENT:
        return "Error No Client";

    case ERROR_INDICATE_TIMEOUT:
        return "Error Indicate Timeout";

    case ERROR_INDICATE_FAILURE:
        return "Error Indicate Failure";

    default:
        return "Unkown";
    }
}