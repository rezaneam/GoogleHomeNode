using System;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;

namespace ConfigTool.BLE
{
    public class DeviceService : ServiceBase
    {
        public DeviceService() : base(SupportedUuids.UUID_INF_SERV)
        {
            localValueChanged = new TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs>(async (GattCharacteristic sender, GattValueChangedEventArgs args) =>
            {
                await GetCharacteristicValue(sender);
                OnPropertyChanged(nameof(SystemID));
                OnPropertyChanged(nameof(ModelNumber));
                OnPropertyChanged(nameof(SerialNumber));
                OnPropertyChanged(nameof(FirewareNumber));
                OnPropertyChanged(nameof(HardwareNumber));
                OnPropertyChanged(nameof(SoftwareNumber));
                OnPropertyChanged(nameof(Manufacturer));
                if (valueChanged != null)
                    valueChanged.Invoke(sender, args);
            });
        }
        public string SystemID => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_SYSID), out string response) ? response : string.Empty;
        public string ModelNumber => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_MODEL_NR), out string response) ? response : string.Empty;
        public string SerialNumber => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_SERIAL_NR), out string response) ? response : string.Empty;
        public string FirewareNumber => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_FW_NR), out string response) ? response : string.Empty;
        public string HardwareNumber => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_HW_NR), out string response) ? response : string.Empty;
        public string SoftwareNumber => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_SW_NR), out string response) ? response : string.Empty;
        public string Manufacturer => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_MANUF_NR), out string response) ? response : string.Empty;
        //public string Certificate => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_CERT), out string response) ? response : string.Empty;
        //public string PnpID => values.TryGetValue(new Guid(SupportedUuids.UUID_INF_PNP_ID), out string response) ? response : string.Empty;
    }
}
