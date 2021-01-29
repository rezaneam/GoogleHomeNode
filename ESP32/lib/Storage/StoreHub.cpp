#include <StoreHub.h>

std::string Storage_WiFi_SSID;
std::string Storage_WiFi_Pass;
std::string Storage_Home_Name;
std::string Storage_Azure_IoT;

void EraseFlash(bool safe)
{
    if (safe)
    {
        uint8_t erase_len = EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_AZURE_IOT_HUB_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
        for (size_t i = 0; i < erase_len; i++)
            EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + i, 0);
    }

    EEPROM.write(EEPROM_WIFI_SSID_NAME_LEN_ADDR, 0);
    EEPROM.write(EEPROM_WIFI_PASSWORD_LEN_ADDR, 0);
    EEPROM.write(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR, 0);
    EEPROM.write(EEPROM_AZURE_IOT_HUB_LEN_ADDR, 0);

    EEPROM.commit();
}

void ReadFlash()
{
    Storage_WiFi_SSID.clear();
    uint8_t ssid_len = EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
    if (ssid_len == EEPROM_UNINITIALIZED_VALUE)
        ssid_len = 0;
    for (size_t i = 0; i < ssid_len; i++)
        Storage_WiFi_SSID.push_back(EEPROM.readChar(i + EEPROM_STORAGE_START_ADDR));

    Storage_WiFi_Pass.clear();
    uint8_t pass_len = EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
    if (pass_len == EEPROM_UNINITIALIZED_VALUE)
        pass_len = 0;
    for (size_t i = 0; i < pass_len; i++)
        Storage_WiFi_Pass.push_back(EEPROM.readChar(i + ssid_len + EEPROM_STORAGE_START_ADDR));

    Storage_Home_Name.clear();
    uint8_t home_len = EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
    if (home_len == EEPROM_UNINITIALIZED_VALUE)
        home_len = 0;
    for (size_t i = 0; i < home_len; i++)
        Storage_Home_Name.push_back(EEPROM.readChar(i + ssid_len + pass_len + EEPROM_STORAGE_START_ADDR));

    Storage_Azure_IoT.clear();
    uint8_t azure_len = EEPROM.read(EEPROM_AZURE_IOT_HUB_LEN_ADDR);
    if (azure_len == EEPROM_UNINITIALIZED_VALUE)
        azure_len = 0;
    for (size_t i = 0; i < azure_len; i++)
        Storage_Azure_IoT.push_back(EEPROM.readChar(i + ssid_len + pass_len + home_len + EEPROM_STORAGE_START_ADDR));
}

void WriteFlash(std::string wifi_ssid, std::string wifi_pass, std::string home_name, std::string iot_hub_connection_string)
{
    Serial.println("Writing to the flash. SSID: " + String(wifi_ssid.c_str()) + " Password: " + String(wifi_pass.c_str()) + " Home Name: " + String(home_name.c_str()));
    Storage_WiFi_SSID = wifi_ssid;
    Storage_WiFi_Pass = wifi_pass;
    Storage_Home_Name = home_name;
    Storage_Azure_IoT = iot_hub_connection_string;

    uint8_t ssid_len = 0;
    uint8_t pass_len = 0;
    uint8_t home_len = 0;
    uint8_t azure_len = 0;

    if (!wifi_ssid.empty())
        ssid_len = wifi_ssid.length();
    EEPROM.write(EEPROM_WIFI_SSID_NAME_LEN_ADDR, ssid_len);
    for (size_t i = 0; i < ssid_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + i, wifi_ssid.at(i));

    if (!wifi_pass.empty())
        pass_len = wifi_pass.length();
    EEPROM.write(EEPROM_WIFI_PASSWORD_LEN_ADDR, pass_len);
    for (size_t i = 0; i < pass_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + ssid_len + i, wifi_pass.at(i));

    if (!home_name.empty())
        home_len = home_name.length();
    EEPROM.write(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR, home_len);
    for (size_t i = 0; i < home_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + ssid_len + pass_len + i, home_name.at(i));

    if (!iot_hub_connection_string.empty())
        azure_len = iot_hub_connection_string.length();
    EEPROM.write(EEPROM_AZURE_IOT_HUB_LEN_ADDR, azure_len);
    for (size_t i = 0; i < azure_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + ssid_len + pass_len + home_len + i, iot_hub_connection_string.at(i));

    EEPROM.commit();
}

void WriteAzureIoTHub(std::string connection_string)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, Storage_Home_Name, connection_string);
}

void WriteFlashWiFi(std::string wifi_ssid, std::string wifi_pass)
{
    WriteFlash(wifi_ssid, wifi_pass, Storage_Home_Name, Storage_Azure_IoT);
}

void WriteFlashHomeName(std::string home_name)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, home_name, Storage_Azure_IoT);
}

bool HasValidWiFi()
{
    ReadFlash();
    return !(Storage_WiFi_SSID.empty() || Storage_WiFi_Pass.empty());
}

bool HasValidHome()
{
    ReadFlash();
    return !Storage_Home_Name.empty();
}

bool HasValidAzure()
{
    ReadFlash();
    return !Storage_Azure_IoT.empty();
}

std::string GetFlashValue(EEPROM_VALUE value, bool readFlash)
{
    if (readFlash)
        void ReadFlash();
    switch (value)
    {
    case EEPROM_VALUE::WiFi_SSID:
        return Storage_WiFi_SSID;
    case EEPROM_VALUE::WiFi_Password:
        return Storage_WiFi_Pass;
    case EEPROM_VALUE::Google_Home_Name:
        return Storage_Home_Name;
    default:
        return "";
    }
}