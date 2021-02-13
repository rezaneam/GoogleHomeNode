using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;
using Windows.Security.Cryptography;
using System.ComponentModel;
using Windows.Devices.Enumeration;
using System.Diagnostics;
using Windows.Storage.Streams;

namespace ConfigTool.BLE
{
    public class ServiceBase : IDisposable, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private GattDeviceService service;
        private ObservableCollection<GattCharacteristic> characteristics = new ObservableCollection<GattCharacteristic>();
        protected Dictionary<Guid, string> values = new Dictionary<Guid, string>();
        private Guid serviceUuid;
        protected TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> localValueChanged;
        protected TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> valueChanged;

        public ServiceBase(string Uuid)
        {
            serviceUuid = new Guid(Uuid);
        }
        public async Task<bool> Initialize(IReadOnlyList<GattDeviceService> services, TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> eventHandler)
        {
            service = services.FirstOrDefault(item => item.Uuid == serviceUuid);
            valueChanged = eventHandler;
            if (service == null) return false;

            if (await service.RequestAccessAsync() != DeviceAccessStatus.Allowed) return false;

            GattCharacteristicsResult result = await service.GetCharacteristicsAsync(BluetoothCacheMode.Uncached);
            if (result.Status != GattCommunicationStatus.Success) return false;

            foreach (var item in result.Characteristics)
            {
                GattReadResult readResult = await item.ReadValueAsync();
                if (readResult.Status != GattCommunicationStatus.Success) 
                    return false;
                if (item.CharacteristicProperties.HasFlag(GattCharacteristicProperties.Notify))
                {
                    await item.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.Notify);
                    item.ValueChanged += valueChanged;
                    item.ValueChanged += localValueChanged;
                }
                characteristics.Add(item);
                values.Add(item.Uuid, FormatToString(readResult.Value));

            }
            return true;
        }

        public void SetValueChangedEvent(TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> eventValuechanged)
        {
            valueChanged = eventValuechanged;
        }

     
        public void Dispose()
        {
            foreach (var item in characteristics)
                item.ValueChanged -= valueChanged;
            service = null;
            characteristics = null;
            values = null;
            GC.SuppressFinalize(this);
        }

        protected async Task<string> GetCharacteristicValue(Guid characteristicsUuid)
        {
            GattCharacteristic characteristic = characteristics.FirstOrDefault(item => item.Uuid.Equals(characteristicsUuid));
            if (characteristic == null) return null;
            GattReadResult readResult = await characteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
            if (readResult.Status != GattCommunicationStatus.Success) return null;
            string value = FormatToString(readResult.Value);
            values[characteristicsUuid] = value;
            return value;
        }

        protected void OnPropertyChanged(string name)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

        protected async Task<string> GetCharacteristicValue(GattCharacteristic characteristic)
        {
            
            if (characteristic == null) return null;
            GattReadResult readResult = await characteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
            if (readResult.Status != GattCommunicationStatus.Success) return null;
            string value = FormatToString(readResult.Value);
            values[characteristic.Uuid] = value;
            return value;
        }

        protected async Task SetCharacteristicValue(GattCharacteristic characteristic, string value)
        {
            if (characteristic == null) return;
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary(value, BinaryStringEncoding.Utf8);
            var readResult = await characteristic.WriteValueAsync(buffUTF8);
            if (readResult != GattCommunicationStatus.Success) return;
            values[characteristic.Uuid] = value;
        }

        protected async Task SetCharacteristicValue(Guid uuid, string value)
        {
            var characteristic = characteristics.FirstOrDefault(item => item.Uuid == uuid);
            if (characteristic == null) return;
            IBuffer buffUTF8 = CryptographicBuffer.ConvertStringToBinary(value, BinaryStringEncoding.Utf8);
            var readResult = await characteristic.WriteValueAsync(buffUTF8);
            if (readResult != GattCommunicationStatus.Success) return;
            values[characteristic.Uuid] = value;
        }

        private string FormatToString(IBuffer value)
        {
            string formattedResult = string.Empty;
            CryptographicBuffer.CopyToByteArray(value, out byte[] data);
            if (data != null)
                formattedResult = Encoding.UTF8.GetString(data);
            return formattedResult;
        }
    }
}
