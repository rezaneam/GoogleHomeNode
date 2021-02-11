using System;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;

namespace ConfigTool.BLE
{
    public class ConfigurationService : ServiceBase
    {
        public ConfigurationService() : base(SupportedUuids.UUID_CON_SERV)
        {
            localValueChanged = new TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs>(async (GattCharacteristic sender, GattValueChangedEventArgs args) =>
            {
                await GetCharacteristicValue(sender);
                OnPropertyChanged(nameof(ScanStatus));
                OnPropertyChanged(nameof(FoundSSIDs));
                OnPropertyChanged(nameof(SSID));
                OnPropertyChanged(nameof(ConnectionStatus));
            });
        }

        public async Task StartScan()
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_SCAN), "1");
        }

        public async Task ResetNode(string mode)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_REST_FACT), mode);
        }

        public async Task<string> FetchSSIDs()
        {
            return await GetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSIDS));
        }

        public async Task<string> GetConnectionStatus()
        {
            return await GetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_DEVI_CONN));
        }

        public async Task TryConnect(string ssid, string password)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSID), ssid);
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_PASS), password);
            StringBuilder status = new StringBuilder(ConnectionStatus);
            status[0] = '1';
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_DEVI_CONN), status.ToString());
        }

        public async Task SetGoogleHomeName(string name)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_HOME_NAME), name);
        }

        public async Task SetNodeLocation(string name)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_DEVIC_LOC), name);
        }

        public async Task SetUsername(string name)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_USERNAME), name);
        }

        public async Task SetAzureConnectionString(string connection)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_AZURE_IOT), connection);
        }

        public async Task SetDeviceLocation(string location)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_DEVIC_LOC), location);
        }

        public string ScanStatus => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SCAN), out string response) ? response : string.Empty;
        public string FoundSSIDs => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSIDS), out string response) ? response : string.Empty;
        public string SSID => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSID), out string response) ? response : string.Empty;
        public string ConnectionStatus => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_DEVI_CONN), out string response) ? response : string.Empty;
        public string GoogleHomeName => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_HOME_NAME), out string response) ? response : string.Empty;
        public string UserID => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_USERNAME), out string response) ? response : string.Empty;
        public string NodeLocation => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_DEVIC_LOC), out string response) ? response : string.Empty;

    }
}
