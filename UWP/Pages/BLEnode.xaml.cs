using ConfigTool.BLE;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
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
    public sealed partial class BLEnode : Page
    {
        readonly int E_DEVICE_NOT_AVAILABLE = unchecked((int)0x800710df); // HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE)
        Node node;
        public BLEnode()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter == null) return;
            if ((string)e.Parameter == string.Empty) return;
            BLEconnectingStackPanel.Visibility = Visibility;
            node = new Node();
            await ConnectDevice((string)e.Parameter);
        }

        #region Connecting

        private async void onConnectionStatusChanged(BluetoothLEDevice sender, object args)
        {
            if (sender.ConnectionStatus == BluetoothConnectionStatus.Disconnected)
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => 
                {
                    node.Dispose();
                    Frame.Navigate(typeof(HomePage)); 
                });
                
        }

        private async Task ConnectDevice(string deviceId)
        {
            try
            {
                node = new Node();
                if (!await node.InitializeDevice(deviceId, onConnectionStatusChanged))
                    Debug.WriteLine("Failed to connect to device.");
                else
                    if (!await node.InitializeService())
                    Debug.WriteLine("Device unreachable");
                else
                    Debug.WriteLine("Successfully connected to the node");

            }
            catch (Exception ex) when (ex.HResult == E_DEVICE_NOT_AVAILABLE)
            {
                Debug.WriteLine("Bluetooth radio is not on.");
            }

            BLEconnectingStackPanel.Visibility = Visibility.Collapsed;
            DisconnectSensorButton.IsEnabled = true;
            ResetSensorButton.IsEnabled = true;
            ReadSensorButton.IsEnabled = true;
            mainFrame.Navigate(typeof(ConfigurationPage), node);
        }

        #endregion

        #region Reset sensor

        private async void OpenNodeResetOptions(object sender, RoutedEventArgs e)
        {
            await RestNodeContentDialog.ShowAsync();
        }

        private void CancelRest(object sender, RoutedEventArgs e)
        {
            RestNodeContentDialog.Hide();
        }

        private async void OnRestartSensor(object sender, RoutedEventArgs e)
        {
            RestNodeContentDialog.Hide();
            await node.Config.ResetNode("1");
        }

        private async void OnHardReset(object sender, RoutedEventArgs e)
        {
            RestNodeContentDialog.Hide();
            await node.Config.ResetNode("2");
        }

        private async void OnSafeHardResetNode(object sender, RoutedEventArgs e)
        {
            RestNodeContentDialog.Hide();
            await node.Config.ResetNode("3");
        }

        #endregion

        private void onCancelConnect(object sender, RoutedEventArgs e)
        {
            node.Dispose();
            Frame.Navigate(typeof(HomePage));
        }

        private void OnReadSensors(object sender, RoutedEventArgs e)
        {
            ReadSensorButton.IsEnabled = false;
            ConfigNodeButton.IsEnabled = true;
            mainFrame.Navigate(typeof(SensorPage), node);
        }

        private void OnConfigNode(object sender, RoutedEventArgs e)
        {
            ReadSensorButton.IsEnabled = true;
            ConfigNodeButton.IsEnabled = false;
            mainFrame.Navigate(typeof(ConfigurationPage), node);
        }

        private void OnDisconnectNode(object sender, RoutedEventArgs e)
        {
            node.Dispose();
            Frame.Navigate(typeof(HomePage));
        }
    }
}
