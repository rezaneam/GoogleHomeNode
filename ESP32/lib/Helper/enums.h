#pragma once

enum CustomEvents
{
    EVENT_NONE,
    EVENT_BLE_CONNECTED,
    EVENT_BLE_DISCONNECT,
    EVENT_BLE_STOPPED,
    EVENT_BLE_TRY_START_ADV,
    EVENT_WIFI_START_SCAN,
    EVENT_WIFI_TRY_CONNECT,
    EVENT_WIFI_CONNECTED,
    EVENT_WIFI_DISCONNECTED,
    EVENT_GOOGLE_TYPE_CONNECT,
    EVENT_GOOGLE_HOME_CONNECTED,
    EVENT_GOOGLE_HOME_NAME,
    EVENT_GOOGLE_REPORT_TEMPERATURE,
    EVENT_GOOGLE_REPORT_HUMIDITY,
    EVENT_GOOGLE_REPORT_AIR_QUALITY,
    EVENT_GOOGLE_REPORT_PRESSURE,
    EVENT_AZURE_IOT_HUB_TRY_CONNECT,
    EVENT_FACTORY_RESET,
    EVENT_FACTORY_RESET_SAFE,
    EVENT_RESTART,

};