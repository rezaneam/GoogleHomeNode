using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;

namespace ConfigTool.BLE
{
    public class Node : IDisposable
    {
        public DeviceService DeviceInfo = new DeviceService();
        public BatteryService Battery = new BatteryService();
        public SensorService Sensors = new SensorService();
        public ConfigurationService Config = new ConfigurationService();
        public BluetoothLEDevice BLEDevice;

        public event TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> ValueChanged;

        public async Task<bool> InitializeDevice(string deviceId, TypedEventHandler<BluetoothLEDevice, object> connectionEvent)
        {
            // BT_Code: BluetoothLEDevice.FromIdAsync must be called from a UI thread because it may prompt for consent.
            BLEDevice = await BluetoothLEDevice.FromIdAsync(deviceId);
            BLEDevice.ConnectionStatusChanged += connectionEvent;
            return BLEDevice != null;
        }

        

        public async Task<bool> InitializeService()
        {
            /// Note: BluetoothLEDevice.GattServices property will return an empty list for unpaired devices. For all uses we recommend using the GetGattServicesAsync method.
            // BT_Code: GetGattServicesAsync returns a list of all the supported services of the device (even if it's not paired to the system).
            // If the services supported by the device are expected to change during BT usage, subscribe to the GattServicesChanged event.
            GattDeviceServicesResult result = await BLEDevice.GetGattServicesAsync(BluetoothCacheMode.Uncached);
            if (result.Status != GattCommunicationStatus.Success) return false;
            IReadOnlyList<GattDeviceService> services = result.Services;
            if (!await DeviceInfo.Initialize(result.Services, ValueChanged)) return false;
            if (!await Battery.Initialize(result.Services, ValueChanged)) return false;
            if (!await Sensors.Initialize(result.Services, ValueChanged)) return false;
            if (!await Config.Initialize(result.Services, ValueChanged)) return false;
            return true;

        }

        public void SetEvent(TypedEventHandler<GattCharacteristic, GattValueChangedEventArgs> valuechanged)
        {
            DeviceInfo.SetValueChangedEvent(valuechanged);
            Battery.SetValueChangedEvent(valuechanged);
            Sensors.SetValueChangedEvent(valuechanged);
            Config.SetValueChangedEvent(valuechanged);
        }
        
        public void Dispose()
        {
            DeviceInfo.Dispose();
            Battery.Dispose();
            Sensors.Dispose();
            Config.Dispose();
            BLEDevice.Dispose();
            GC.SuppressFinalize(this);
        }


    }
}
