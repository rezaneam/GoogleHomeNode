#include <Wireless.h>

void Wireless::Initialize(void (*event_queue_method)(CustomEvents), char *hostname, bool verbose)
{
    queueEventWiFi = event_queue_method;
    isVerbose = verbose;
    WiFi.mode(WIFI_STA);
    if (hostname != "")
        WiFi.setHostname(hostname);
}

bool Wireless::ScanNodes()
{

    int n = WiFi.scanNetworks();
    SSIDs.clear();
    if (n == 0)
    {
        if (isVerbose)
            printf("no networks found.\r\n");

        return false;
    }
    else
    {
        if (isVerbose)
            printf("%d networks found.\r\n", n);
        for (int i = 0; i < n; ++i)
        {
            if (isVerbose)
                printf("%d - %s (%ddbm) [%d] %d %s \r\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i), WiFi.encryptionType(i), translateEncryptionType(WiFi.encryptionType(i)));

            // Print SSID and RSSI for each network found
            for (size_t j = 0; j < WiFi.SSID(i).length(); j++)
                SSIDs.push_back(WiFi.SSID(i)[j]);

            if (i + 1 != n)
                SSIDs.push_back(',');
            delay(10);
        }
        queueEventWiFi(CustomEvents::EVENT_WIFI_SCAN_COMPLETED);
        return true;
    }
}
bool Wireless::TryConnect(std::string ssid, std::string password)
{
    if (isVerbose)
        printf("Trying to connect to %s\r\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    uint8_t connectionStatus = WiFi.waitForConnectResult();
    if (connectionStatus == WL_CONNECTED)
    {
        queueEventWiFi(CustomEvents::EVENT_WIFI_CONNECTED);
        if (isVerbose)
            printf("IP: %s   Hostname: %s   Gateway IP: %s\r\n", WiFi.localIP().toString().c_str(), WiFi.getHostname(), WiFi.gatewayIP().toString().c_str());
        gatewayIP = WiFi.gatewayIP();
        localIP = WiFi.localIP();
        isConnected = true;
        return true;
    }
    if (isVerbose)
    {
        if (connectionStatus == WL_NO_SSID_AVAIL)
            printf("WiFi connection failed. SSID is not available.\r\n");
        else
            printf("WiFi Connection failed. Code %d\r\n", connectionStatus);
    }
    queueEventWiFi(CustomEvents::EVENT_WIFI_DISCONNECTED);
    WiFi.disconnect();
    isConnected = false;
    return false;
}

bool Wireless::IsOnline()
{
    if (!isConnected)
        return false;
    return Ping.ping("www.google.com", 4);
}

bool Wireless::IsConnected()
{
    if (!isConnected)
        return false;
    return Ping.ping(gatewayIP, 4);
}

String Wireless::translateEncryptionType(wifi_auth_mode_t encryptionType)
{
    switch (encryptionType)
    {
    case (0):
        return "Open";
    case (1):
        return "WEP";
    case (2):
        return "WPA PSK";
    case (3):
        return "WPA2 PSK";
    case (4):
        return "WPA/2 PSK";
    case (5):
        return "WPA2 ENTERPRISE";
    default:
        return "UNKOWN";
    }
}