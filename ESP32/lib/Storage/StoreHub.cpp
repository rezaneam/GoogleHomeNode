#include <StoreHub.h>

std::string Storage_WiFi_SSID;
std::string Storage_WiFi_Pass;
std::string Storage_Home_Name;
std::string Storage_Azure_IoT;
std::string Storage_Dev_Locat;
std::string Storage_User_Name;

void EraseFlash(bool safe)
{
    if (safe)
    {
        uint8_t erase_len = EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_AZURE_IOT_HUB_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_DEVICE_LOCATION_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_USER_NAME_LEN_ADDR);
        erase_len += EEPROM.read(EEPROM_BSEC_STAT_LEN_ADDR);
        for (size_t i = 0; i < erase_len; i++)
            EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + i, 0);
    }

    EEPROM.write(EEPROM_WIFI_SSID_NAME_LEN_ADDR, 0);
    EEPROM.write(EEPROM_WIFI_PASSWORD_LEN_ADDR, 0);
    EEPROM.write(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR, 0);
    EEPROM.write(EEPROM_AZURE_IOT_HUB_LEN_ADDR, 0);
    EEPROM.write(EEPROM_DEVICE_LOCATION_LEN_ADDR, 0);
    EEPROM.write(EEPROM_USER_NAME_LEN_ADDR, 0);
    EEPROM.write(EEPROM_BSEC_STAT_LEN_ADDR, 0);

    EEPROM.commit();
}

void ReadFlash()
{
    uint8_t pos = 0;
    uint8_t len = 0;
    Storage_WiFi_SSID.clear();
    len = EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_WiFi_SSID.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;

    Storage_WiFi_Pass.clear();
    len = EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_WiFi_Pass.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;

    Storage_Home_Name.clear();
    len = EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_Home_Name.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;

    Storage_Azure_IoT.clear();
    len = EEPROM.read(EEPROM_AZURE_IOT_HUB_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_Azure_IoT.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;

    Storage_Dev_Locat.clear();
    len = EEPROM.read(EEPROM_DEVICE_LOCATION_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_Dev_Locat.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;

    Storage_User_Name.clear();
    len = EEPROM.read(EEPROM_USER_NAME_LEN_ADDR);
    if (len == EEPROM_UNINITIALIZED_VALUE)
        len = 0;
    for (size_t i = 0; i < len; i++)
        Storage_User_Name.push_back(EEPROM.readChar(i + pos + EEPROM_STORAGE_START_ADDR));
    pos += len;
}

uint16_t getBSECstorePosition()
{
    uint16_t pos = EEPROM_STORAGE_START_ADDR;

    pos += EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
    pos += EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
    pos += EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
    pos += EEPROM.read(EEPROM_AZURE_IOT_HUB_LEN_ADDR);
    pos += EEPROM.read(EEPROM_DEVICE_LOCATION_LEN_ADDR);
    pos += EEPROM.read(EEPROM_USER_NAME_LEN_ADDR);

    return pos;
}

bool GetBSECstate(uint8_t *state, uint8_t expected_len)
{
    uint16_t pos = getBSECstorePosition();
    uint8_t len = EEPROM.read(EEPROM_BSEC_STAT_LEN_ADDR);
    if (len != expected_len)
    {
        printf(">> Critical Error! BSEC state load. Length doesn't match. Actual %d but expected %d\r\n", len, expected_len);
        return false;
    }
    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t val = EEPROM.read(i + pos);
        *(state + i) = val;
        printf("%X ", val);
    }
    printf("\r\n");
    return true;
}

bool HasBSECstate()
{
    uint8_t len = EEPROM.read(EEPROM_BSEC_STAT_LEN_ADDR);
    return ((len != 0) & (len != EEPROM_UNINITIALIZED_VALUE));
}

void WriteFlashBSECstate(uint8_t *state, uint8_t len)
{
    uint16_t pos = getBSECstorePosition();
    EEPROM.write(EEPROM_BSEC_STAT_LEN_ADDR, len);
    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t val = *(state + i);
        EEPROM.write(i + pos, val);
        printf("%X ", val);
    }
    printf("\r\n");
    EEPROM.commit();
}

void WriteFlash(std::string wifi_ssid, std::string wifi_pass, std::string home_name, std::string iot_hub_connection_string, std::string device_location, std::string username)
{
    Storage_WiFi_SSID = wifi_ssid;
    Storage_WiFi_Pass = wifi_pass;
    Storage_Home_Name = home_name;
    Storage_Azure_IoT = iot_hub_connection_string;
    Storage_Dev_Locat = device_location;
    Storage_User_Name = username;

    uint8_t len = 0;
    uint8_t pos = 0;

    // Reading possible BSEC stat from FLASH
    uint8_t stat_len = EEPROM.read(EEPROM_BSEC_STAT_LEN_ADDR);
    if (stat_len == EEPROM_UNINITIALIZED_VALUE)
        stat_len = 0;
    uint16_t stat_pos = getBSECstorePosition();
    uint8_t bsecState[stat_len] = {0};
    for (uint8_t i = 0; i < stat_len; i++)
        bsecState[i] = EEPROM.read(i + stat_pos);

    len = !wifi_ssid.empty() ? wifi_ssid.length() : 0;
    EEPROM.write(EEPROM_WIFI_SSID_NAME_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, wifi_ssid.at(i));
    pos += len;

    len = !wifi_pass.empty() ? wifi_pass.length() : 0;
    EEPROM.write(EEPROM_WIFI_PASSWORD_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, wifi_pass.at(i));
    pos += len;

    len = !home_name.empty() ? home_name.length() : 0;
    EEPROM.write(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, home_name.at(i));
    pos += len;

    len = !iot_hub_connection_string.empty() ? iot_hub_connection_string.length() : 0;
    EEPROM.write(EEPROM_AZURE_IOT_HUB_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, iot_hub_connection_string.at(i));
    pos += len;

    len = !device_location.empty() ? device_location.length() : 0;
    EEPROM.write(EEPROM_DEVICE_LOCATION_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, device_location.at(i));
    pos += len;

    len = !username.empty() ? username.length() : 0;
    EEPROM.write(EEPROM_USER_NAME_LEN_ADDR, len);
    for (size_t i = 0; i < len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + pos + i, username.at(i));
    pos += len;

    // Re-writing the read BSEC stat
    for (uint8_t i = 0; i < stat_len; i++)
        EEPROM.write(EEPROM_STORAGE_START_ADDR + pos + i, bsecState[i]);

    EEPROM.commit();
}

void WriteFlashAzureIoTHub(std::string connection_string)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, Storage_Home_Name, connection_string, Storage_Dev_Locat, Storage_User_Name);
}

void WriteFlashWiFi(std::string wifi_ssid, std::string wifi_pass)
{
    WriteFlash(wifi_ssid, wifi_pass, Storage_Home_Name, Storage_Azure_IoT, Storage_Dev_Locat, Storage_User_Name);
}

void WriteFlashHomeName(std::string home_name)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, home_name, Storage_Azure_IoT, Storage_Dev_Locat, Storage_User_Name);
}

void WriteFlashUsername(std::string username)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, Storage_Home_Name, Storage_Azure_IoT, Storage_Dev_Locat, username);
}

void WriteFlashDeviceLocation(std::string device_location)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, Storage_Home_Name, Storage_Azure_IoT, device_location, Storage_User_Name);
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

bool HasUsername()
{
    ReadFlash();
    return !Storage_User_Name.empty();
}

bool HasDeviceLocation()
{
    ReadFlash();
    return !Storage_Dev_Locat.empty();
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
    case EEPROM_VALUE::Azure_IoT_Hub:
        return Storage_Azure_IoT;
    case EEPROM_VALUE::User_Name:
        return Storage_User_Name;
    case EEPROM_VALUE::Device_Location:
        return Storage_Dev_Locat;
    default:
        return "";
    }
}