﻿using ConfigTool.BLE;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.Radios;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace Config_Tool___Google_Home_Node.Pages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class HomePage : Page
    {
        

        public HomePage()
        {
            this.InitializeComponent();
            Loaded += HomePage_Loaded;
        }

        private async void HomePage_Loaded(object sender, RoutedEventArgs e)
        {
            IReadOnlyList<Radio> radios = await Radio.GetRadiosAsync();
            Radio bluetoothRadio = radios.FirstOrDefault(radio => radio.Kind == RadioKind.Bluetooth);
            if (bluetoothRadio == null)
            {
                StatusMsgTextBox.Text = "Your device doesn't equipped with Bluetooth";
                return;
            }
            if (bluetoothRadio.State != RadioState.On)
                StatusMsgTextBox.Text = "Please turn on your Bluetooth";

            FindNodeButton.IsEnabled = bluetoothRadio.State == RadioState.On;
            bluetoothRadio.StateChanged += BluetoothRadio_StateChanged;
        }

        private void BluetoothRadio_StateChanged(Radio sender, object args)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
            {
                FindNodeButton.IsEnabled = sender.State == RadioState.On;
                if (sender.State == RadioState.On)
                    StatusMsgTextBox.Text = "Ready to scan";
                else
                    StatusMsgTextBox.Text = "Please turn on your Bluetooth";
            });
            
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter == null) return;
            if ((string)e.Parameter == null || string.IsNullOrEmpty((string)e.Parameter)) return;
            var messageDialog = new MessageDialog((string)e.Parameter, "Error!");
            messageDialog.Commands.Add(new UICommand("Got it.") { Id = 0 });
            messageDialog.CancelCommandIndex = 0;
            await messageDialog.ShowAsync();
        }

        #region UI 
        private void OnCancelSearchNodes(object sender, RoutedEventArgs e)
        {
            BLEScanContentDialog.Hide();
            StopBleDeviceWatcher();
        }

        private async void OnSearchNodes(object sender, RoutedEventArgs e)
        {
            StartBleDeviceWatcher();
            ScanBLEstackPanel.Visibility = Visibility.Visible;
            await BLEScanContentDialog.ShowAsync();
        }

        private void OnConnectDevice(object sender, RoutedEventArgs e)
        {
            BLEScanContentDialog.Hide();
            StopBleDeviceWatcher();
            var bluetoothDevice = (sender as Button).DataContext as BluetoothLEDeviceDisplay;
            Frame.Navigate(typeof(BLEnode), bluetoothDevice.Id);
        }

        #endregion

        #region Device discovery
        public ObservableCollection<BluetoothLEDeviceDisplay> KnownDevices = new ObservableCollection<BluetoothLEDeviceDisplay>();
        public string FoundDevices => KnownDevices.Count.ToString();
        public ObservableCollection<DeviceInformation> UnknownDevices = new ObservableCollection<DeviceInformation>();
        private DeviceWatcher deviceWatcher;

        /// <summary>
        /// Starts a device watcher that looks for all nearby Bluetooth devices (paired or unpaired). 
        /// Attaches event handlers to populate the device collection.
        /// </summary>
        public void StartBleDeviceWatcher()
        {
            IsSearchingProgressRing.IsActive = true;
            SearchStatusTextBlock.Text = "Searching for nodes";
            // Additional properties we would like about the device.
            // Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
            string[] requestedProperties = {
                "System.Devices.Aep.DeviceAddress",
                "System.Devices.Aep.IsConnected", "" +
                "System.Devices.Aep.Bluetooth.Le.IsConnectable",
                "System.Devices.Aep.Category",
                "System.Devices.Aep.ContainerId",
                "System.Devices.Aep.DeviceAddress",
                "System.Devices.Aep.IsPaired",
                "System.Devices.Aep.IsPresent",
                "System.Devices.Aep.Manufacturer",
                "System.Devices.Aep.ModelId",
                "System.Devices.Aep.ModelName",
                "System.Devices.Aep.SignalStrength"
            };

            // BT_Code: Example showing paired and non-paired in a single query.
            string aqsAllBluetoothLEDevices = "(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

            deviceWatcher =
                    DeviceInformation.CreateWatcher(
                        aqsAllBluetoothLEDevices,
                        requestedProperties,
                        DeviceInformationKind.AssociationEndpoint);

            // Register event handlers before starting the watcher.
            deviceWatcher.Added += DeviceWatcher_Added;
            deviceWatcher.Updated += DeviceWatcher_Updated;
            deviceWatcher.Removed += DeviceWatcher_Removed;
            deviceWatcher.EnumerationCompleted += DeviceWatcher_EnumerationCompleted;
            deviceWatcher.Stopped += DeviceWatcher_Stopped;

            // Start over with an empty collection.
            KnownDevices.Clear();

            // Start the watcher. Active enumeration is limited to approximately 30 seconds.
            // This limits power usage and reduces interference with other Bluetooth activities.
            // To monitor for the presence of Bluetooth LE devices for an extended period,
            // use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
            // sample for an example.
            deviceWatcher.Start();
        }

        /// <summary>
        /// Stops watching for all nearby Bluetooth devices.
        /// </summary>
        public void StopBleDeviceWatcher()
        {
            if (deviceWatcher != null)
            {
                // Unregister the event handlers.
                deviceWatcher.Added -= DeviceWatcher_Added;
                deviceWatcher.Updated -= DeviceWatcher_Updated;
                deviceWatcher.Removed -= DeviceWatcher_Removed;
                deviceWatcher.EnumerationCompleted -= DeviceWatcher_EnumerationCompleted;
                deviceWatcher.Stopped -= DeviceWatcher_Stopped;

                // Stop the watcher.
                deviceWatcher.Stop();
                deviceWatcher = null;
            }
        }

        public async Task<DevicePairingResult> Pair(BluetoothLEDeviceDisplay bleDeviceDisplay)
        {
            return await bleDeviceDisplay.DeviceInformation.Pairing.PairAsync();
        }

        private BluetoothLEDeviceDisplay FindBluetoothLEDeviceDisplay(string id)
        {
            foreach (BluetoothLEDeviceDisplay bleDeviceDisplay in KnownDevices)
                if (bleDeviceDisplay.Id == id)
                    return bleDeviceDisplay;

            return null;
        }

        private DeviceInformation FindUnknownDevices(string id)
        {
            foreach (DeviceInformation bleDeviceInfo in UnknownDevices)
            {
                if (bleDeviceInfo.Id == id)
                {
                    return bleDeviceInfo;
                }
            }
            return null;
        }

        private async void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            // We must update the collection on the UI thread because the collection is databound to a UI element.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                lock (this)
                {
                    Debug.WriteLine(String.Format("Added {0}{1}", deviceInfo.Id, deviceInfo.Name));

                    // Protect against race condition if the task runs after the app stopped the deviceWatcher.
                    if (sender == deviceWatcher)
                    {
                        // Make sure device isn't already present in the list.
                        if (FindBluetoothLEDeviceDisplay(deviceInfo.Id) == null)
                        {
                            var tmp = deviceInfo.Properties.ToList();
                            if (deviceInfo.Name != string.Empty)
                            {
                                // If device has a friendly name display it immediately.
                                KnownDevices.Add(new BluetoothLEDeviceDisplay(deviceInfo));
                            }
                            else
                            {
                                // Add it to a list in case the name gets updated later. 
                                UnknownDevices.Add(deviceInfo);
                            }
                        }

                    }
                }
            });
        }

        private async void DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            // We must update the collection on the UI thread because the collection is databound to a UI element.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                lock (this)
                {
                    Debug.WriteLine(String.Format("Updated {0}{1}", deviceInfoUpdate.Id, ""));

                    // Protect against race condition if the task runs after the app stopped the deviceWatcher.
                    if (sender == deviceWatcher)
                    {
                        BluetoothLEDeviceDisplay bleDeviceDisplay = FindBluetoothLEDeviceDisplay(deviceInfoUpdate.Id);
                        if (bleDeviceDisplay != null)
                        {
                            // Device is already being displayed - update UX.
                            bleDeviceDisplay.Update(deviceInfoUpdate);
                            return;
                        }

                        DeviceInformation deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id);
                        if (deviceInfo != null)
                        {
                            deviceInfo.Update(deviceInfoUpdate);
                            // If device has been updated with a friendly name it's no longer unknown.
                            if (deviceInfo.Name != String.Empty)
                            {
                                KnownDevices.Add(new BluetoothLEDeviceDisplay(deviceInfo));
                                UnknownDevices.Remove(deviceInfo);
                            }
                        }
                    }
                }
            });
        }

        private async void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
        {
            // We must update the collection on the UI thread because the collection is databound to a UI element.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                lock (this)
                {
                    Debug.WriteLine(String.Format("Removed {0}{1}", deviceInfoUpdate.Id, ""));

                    // Protect against race condition if the task runs after the app stopped the deviceWatcher.
                    if (sender == deviceWatcher)
                    {
                        // Find the corresponding DeviceInformation in the collection and remove it.
                        BluetoothLEDeviceDisplay bleDeviceDisplay = FindBluetoothLEDeviceDisplay(deviceInfoUpdate.Id);
                        if (bleDeviceDisplay != null)
                            KnownDevices.Remove(bleDeviceDisplay);

                        DeviceInformation deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id);
                        if (deviceInfo != null)
                            UnknownDevices.Remove(deviceInfo);
                    }
                }
            });
        }

        private async void DeviceWatcher_EnumerationCompleted(DeviceWatcher sender, object e)
        {
            // We must update the collection on the UI thread because the collection is databound to a UI element.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                IsSearchingProgressRing.IsActive = false;
                SearchStatusTextBlock.Text = "Search completed";
                // Protect against race condition if the task runs after the app stopped the deviceWatcher.
                if (sender == deviceWatcher)
                    Debug.WriteLine($"{KnownDevices.Count} devices found. Enumeration completed.");
            });
        }

        private async void DeviceWatcher_Stopped(DeviceWatcher sender, object e)
        {
            // We must update the collection on the UI thread because the collection is databound to a UI element.
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Protect against race condition if the task runs after the app stopped the deviceWatcher.
                if (sender == deviceWatcher)
                    Debug.WriteLine($"No longer watching for devices.");
            });
        }
        #endregion

        #region Pairing

        private bool isBusy = false;

        private async void PairDevice(BluetoothLEDeviceDisplay device)
        {
            // Do not allow a new Pair operation to start if an existing one is in progress.
            if (isBusy)
            {
                return;
            }

            isBusy = true;

            Debug.WriteLine("Pairing started. Please wait...");

            // For more information about device pairing, including examples of
            // customizing the pairing process, see the DeviceEnumerationAndPairing sample.

            // BT_Code: Pair the currently selected device.
            DevicePairingResult result = await device.DeviceInformation.Pairing.PairAsync();
            Debug.WriteLine($"Pairing result = {result.Status}");

            isBusy = false;
        }

        #endregion

        
    }
}
