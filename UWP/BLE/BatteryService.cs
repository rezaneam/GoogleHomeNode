using System;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;

namespace ConfigTool.BLE
{
    public class BatteryService : ServiceBase
    {
        public BatteryService() : base(SupportedUuids.UUID_BAT_SERV)
        {
            localValueChanged = new TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs>(async (GattCharacteristic sender, GattValueChangedEventArgs args) =>
            {
                await GetCharacteristicValue(sender);
                OnPropertyChanged(nameof(BatteryLevel));
                valueChanged?.Invoke(sender, args);
            });
        }

        public string BatteryLevel => values.TryGetValue(new Guid(SupportedUuids.UUID_BAT_LVL), out string response) ? response : string.Empty;
    }
}
