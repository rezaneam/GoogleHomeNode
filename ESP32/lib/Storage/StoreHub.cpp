#include <StoreHub.h>

std::string Storage_WiFi_SSID;
std::string Storage_WiFi_Pass;
std::string Storage_Home_Name;

void ReadFlash()
{
    Storage_WiFi_SSID.clear();
    uint8_t ssid_len = EEPROM.read(EEPROM_WIFI_SSID_NAME_LEN_ADDR);
    if(ssid_len == EEPROM_UNINITIALIZED_VALUE) ssid_len = 0;
    Serial.println("SSID length is " + String(ssid_len));
    for (size_t i = 0; i < ssid_len; i++)
        Storage_WiFi_SSID.push_back(EEPROM.readChar(i + EEPROM_STORAGE_START_ADDR));

    Storage_WiFi_Pass.clear();
    uint8_t pass_len = EEPROM.read(EEPROM_WIFI_PASSWORD_LEN_ADDR);
    if(pass_len == EEPROM_UNINITIALIZED_VALUE) pass_len = 0;
    Serial.println("Password length is " + String(pass_len));
    for (size_t i = 0; i < pass_len; i++)
        Storage_WiFi_Pass.push_back(EEPROM.readChar(i + ssid_len + EEPROM_STORAGE_START_ADDR));

    Storage_Home_Name.clear();
    uint8_t home_len = EEPROM.read(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR);
    if(home_len == EEPROM_UNINITIALIZED_VALUE) home_len = 0;
    Serial.println("Home Name length is " + String(home_len));
    for (size_t i = 0; i < home_len; i++)
        Storage_Home_Name.push_back(EEPROM.readChar(i + ssid_len + pass_len + EEPROM_STORAGE_START_ADDR));
}

void WriteFlash(std::string wifi_ssid, std::string wifi_pass, std::string home_name)
{
    Serial.println("Writing to the flash. SSID: " + String(wifi_ssid.c_str()) + " Password: " + String(wifi_pass.c_str()) + " Home Name: " + String(home_name.c_str()));
    Storage_WiFi_SSID = wifi_ssid;
    Storage_WiFi_Pass = wifi_pass;
    Storage_Home_Name = home_name;

    uint8_t ssid_len = 0;
    uint8_t pass_len = 0;
    uint8_t home_len = 0;

    if (!wifi_ssid.empty())
        ssid_len = wifi_ssid.length();
    Serial.println("SSID length is " + String(ssid_len));
    EEPROM.write(EEPROM_WIFI_SSID_NAME_LEN_ADDR, ssid_len);
    for (size_t i = 0; i < ssid_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + i, wifi_ssid.at(i));

    if (!wifi_pass.empty())
        pass_len = wifi_pass.length();
    Serial.println("Password length is " + String(pass_len));
    EEPROM.write(EEPROM_WIFI_PASSWORD_LEN_ADDR, pass_len);
    for (size_t i = 0; i < pass_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + ssid_len + i, wifi_pass.at(i));

    if (!home_name.empty())
        home_len = home_name.length();
    Serial.println("Home Name length is " + String(home_len));
    EEPROM.write(EEPROM_GOOGLE_HOME_NAME_LEN_ADDR, home_len);
    for (size_t i = 0; i < home_len; i++)
        EEPROM.writeChar(EEPROM_STORAGE_START_ADDR + ssid_len + pass_len + i, home_name.at(i));

    EEPROM.commit();
}

void WriteFlashWiFi(std::string wifi_ssid, std::string wifi_pass)
{
    WriteFlash(wifi_ssid, wifi_pass, Storage_Home_Name);
}

void WriteFlashHomeName(std::string home_name)
{
    WriteFlash(Storage_WiFi_SSID, Storage_WiFi_Pass, home_name);
}

bool HasValidWiFi()
{
    ReadFlash();
    return !(Storage_WiFi_SSID.empty() || Storage_WiFi_Pass.empty());
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
    case EEPROM_VALUE::Home_Name:
        return Storage_Home_Name;
    default:
        return "";
    }
}