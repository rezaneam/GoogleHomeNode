using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConfigTool.BLE
{
    public static class SupportedUuids
    {
        public const string UUID_INF_SERV =  "0000180a-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_SYSID = "00002a23-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_MODEL_NR = "00002a24-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_SERIAL_NR = "00002a25-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_FW_NR = "00002a26-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_HW_NR = "00002a27-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_SW_NR = "00002a28-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_MANUF_NR = "00002a29-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_CERT = "00002A2a-0000-1000-8000-00805f9b34fb";
        public const string UUID_INF_PNP_ID = "00002a50-0000-1000-8000-00805f9b34fb";

        public const string UUID_BAT_SERV = "0000180f-0000-1000-8000-00805f9b34fb";
        public const string UUID_BAT_LVL = "00002a19-0000-1000-8000-00805f9b34fb";

        public const string UUID_SEN_SERV = "0000181a-0000-1000-8000-00805f9b34fb";
        public const string UUID_SEN_TMP = "00002a6e-0000-1000-8000-00805f9b34fb";
        public const string UUID_SEN_HUM = "00002a6f-0000-1000-8000-00805f9b34fb";
        public const string UUID_SEN_BAR = "00002a6d-0000-1000-8000-00805f9b34fb";

        public const string UUID_CON_SERV = "0000181c-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_REST_FACT = "00004900-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_WIFI_SSIDS = "00004901-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_WIFI_SCAN = "00004902-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_WIFI_SSID = "00004903-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_WIFI_PASS = "00004904-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_WIFI_CONN = "00004905-0000-1000-8000-00805f9b34fb";
        public const string UUID_CON_HOME_NAME = "00004906-0000-1000-8000-00805f9b34fb";

        private static Dictionary<Guid, string> supportedServices = new Dictionary<Guid, string>()
        {
            { new Guid(UUID_INF_SERV), "Device Information"},
            { new Guid(UUID_BAT_SERV), "Battery Service"},
            { new Guid(UUID_SEN_SERV), "Sensor Service" },
            { new Guid(UUID_CON_SERV), "Configuration Service" }
        };

        private static Dictionary<Guid, string> knownUuids = new Dictionary<Guid, string>()
        {
            { new Guid(UUID_INF_SERV), "Device Information"},
            { new Guid(UUID_INF_SYSID), "System ID" },
            { new Guid(UUID_INF_MODEL_NR), "Model number" },
            { new Guid(UUID_INF_SERIAL_NR), "Serial number" },
            { new Guid(UUID_INF_FW_NR), "Firmware number" },
            { new Guid(UUID_INF_HW_NR), "Hardware number" },
            { new Guid(UUID_INF_SW_NR), "Software number" },
            { new Guid(UUID_INF_MANUF_NR), "Manufacturer" },
            { new Guid(UUID_INF_CERT), "Certificate" },
            { new Guid(UUID_INF_PNP_ID), "PNP ID" },
            { new Guid(UUID_BAT_LVL), "Battery level" },
            { new Guid(UUID_SEN_TMP), "Temperature" },
            { new Guid(UUID_SEN_BAR), "Barometer" },
            { new Guid(UUID_SEN_HUM), "Humidity" },
            { new Guid(UUID_BAT_SERV), "Battery Service"},
            { new Guid(UUID_SEN_SERV), "Sensor Service" },
            { new Guid(UUID_CON_SERV), "Configuration Service" },
            { new Guid(UUID_CON_REST_FACT), "Restarting and Factory Resting device" },
            { new Guid(UUID_CON_WIFI_SSIDS), "Found WiFi SSIDs" },
            { new Guid(UUID_CON_WIFI_SCAN), "WiFi scan status" },
            { new Guid(UUID_CON_WIFI_SSID), "WiFi SSID" },
            { new Guid(UUID_CON_WIFI_PASS), "WiFi Password" },
            { new Guid(UUID_CON_WIFI_CONN), "WiFi connection status" },
            { new Guid(UUID_CON_HOME_NAME), "Google Home Name" }
        };

        public static string TranslateUuid(Guid uuid)
        {
            if (knownUuids.TryGetValue(uuid, out string name))
                return name;
            return uuid.ToString();
        }

        public static bool IsSupportService(Guid uuid)
        {
            return supportedServices.TryGetValue(uuid, out _);
        }

        public static string FormatToString(Windows.Storage.Streams.IBuffer value)
        {
            string formattedResult = string.Empty;
            Windows.Security.Cryptography.CryptographicBuffer.CopyToByteArray(value, out byte[] data);
            if (data != null)
                formattedResult = Encoding.UTF8.GetString(data);
            return formattedResult;
        }


    }
}
