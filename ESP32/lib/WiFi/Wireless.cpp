#include <Wireless.h>

bool WiFiScanNodes()
{
    WiFi.mode(WIFI_STA);
    int n = WiFi.scanNetworks();
    std::string SSIDs;
    if (n == 0)
    {
        Serial.println("no networks found");
        return false;
    }
    else
    {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            for (size_t j = 0; j < WiFi.SSID(i).length(); j++)
            {
                SSIDs.push_back(WiFi.SSID(i)[j]);
            }
            SSIDs.push_back(',');

            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.print(" [");
            Serial.print(WiFi.channel(i));
            Serial.print("] ");
            String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
            Serial.println(encryptionTypeDescription);
            delay(10);
            BLEsetSSIDs(SSIDs);
        }
        return true;
    }
}
bool WiFiConnect(std::string ssid, std::string password)
{
    BLEwirelessConnectionChanged(BLE_WIFI_CONNECTING);
    Serial.print("Conneting to WiFi. ");
    Serial.print( ssid.c_str() );
    Serial.print( " " );
    Serial.println( password.c_str() );
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED)
        ;
    if (WiFi.status() == WL_CONNECTED)
    {
        BLEwirelessConnectionChanged(BLE_WIFI_CONNECTED);
        Serial.println(WiFi.localIP());
        Serial.println(WiFi.getHostname());
        Serial.println(WiFi.gatewayIP());
        return true;
    }
    Serial.println(WiFi.status());
    BLEwirelessConnectionChanged(BLE_WIFI_NOT_CONNECTED);
    return false;
}
String translateEncryptionType(wifi_auth_mode_t encryptionType)
{
    switch (encryptionType)
    {
    case (0):
        return "Open";
    case (1):
        return "WEP";
    case (2):
        return "WPA_PSK";
    case (3):
        return "WPA2_PSK";
    case (4):
        return "WPA_WPA2_PSK";
    case (5):
        return "WPA2_ENTERPRISE";
    default:
        return "UNKOWN";
    }
}