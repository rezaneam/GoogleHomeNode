using System;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;

namespace ConfigTool.BLE
{
    public class SensorService : ServiceBase
    {
        public SensorService() : base(SupportedUuids.UUID_SEN_SERV)
        {
            localValueChanged = new TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs>(async (GattCharacteristic sender, GattValueChangedEventArgs args) =>
            {
                await GetCharacteristicValue(sender);
                OnPropertyChanged(nameof(Temperature));
                OnPropertyChanged(nameof(Pressure));
                OnPropertyChanged(nameof(Humidity));
                OnPropertyChanged(nameof(AirQualityIndex));
                valueChanged?.Invoke(sender, args);
            });
        }

        public string Temperature => values.TryGetValue(new Guid(SupportedUuids.UUID_SEN_TMP), out string response) ? response : string.Empty;
        public string Humidity => values.TryGetValue(new Guid(SupportedUuids.UUID_SEN_HUM), out string response) ? response : string.Empty;
        public string Pressure => values.TryGetValue(new Guid(SupportedUuids.UUID_SEN_BAR), out string response) ? response : string.Empty;
        public string AirQualityIndex => values.TryGetValue(new Guid(SupportedUuids.UUID_SEN_AQI), out string response) ? response : string.Empty;
    }
}
