using System;
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

        public async Task TryConnect(string ssid, string password)
        {
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSID), ssid);
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_PASS), password);
            await SetCharacteristicValue(new Guid(SupportedUuids.UUID_CON_WIFI_CONN), "1");
        }

        public string ScanStatus => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SCAN), out string response) ? response : string.Empty;
        public string FoundSSIDs => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSIDS), out string response) ? response : string.Empty;
        public string SSID => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_SSID), out string response) ? response : string.Empty;
        public string ConnectionStatus => values.TryGetValue(new Guid(SupportedUuids.UUID_CON_WIFI_CONN), out string response) ? response : string.Empty;
    }
}
