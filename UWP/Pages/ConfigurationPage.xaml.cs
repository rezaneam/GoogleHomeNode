using ConfigTool.BLE;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Security.Cryptography;
using Windows.UI;
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
    public sealed partial class ConfigurationPage : Page
    {
        Node node;
        public ObservableCollection<string> FoundSSIDs = new ObservableCollection<string>();
        public ConfigurationPage()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter == null) return;
            if ((Node)e.Parameter == null) return;
            node = (Node)e.Parameter;
            node.SetEvent(GattCharacteristic_ValueChanged);
            await OnConfigNode();
        }

        private async Task OnConfigNode()
        {
            if (node.Config.ConnectionStatus[0] != '2')
                CurrentWiFiStatusTextBlock.Text = "";
            else
                CurrentWiFiStatusTextBlock.Text = node.Config.SSID;

            GoogleHomeNameTextBox.Text = node.Config.GoogleHomeName;

            UpdateConntionStatus(await node.Config.GetConnectionStatus());

            UsernameTextBox.Text = node.Config.UserID;
            DeviceLocationTextBox.Text = node.Config.NodeLocation;
        }

        #region UI


        private async void OnFindWiFi(object sender, RoutedEventArgs e)
        {
            FoundSSIDs.Clear();
            ScanningProgressRing.IsActive = true;
            ScanningStatusTextBox.Text = "Please wait, Scanning WiFi ...";
            await node.Config.StartScan();
            await WiFiScanContentDialog.ShowAsync();
        }

        private async void onConnectWiFi(object sender, RoutedEventArgs e)
        {
            if (!string.IsNullOrEmpty(SSIDtextBlock.Text) & !string.IsNullOrEmpty(passwordBox.Password))
                await node.Config.TryConnect(SSIDtextBlock.Text, passwordBox.Password);
        }

        private void FoundSSIDsListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PickSSIDButton.IsEnabled = FoundSSIDsListView.SelectedIndex > -1;
        }

        private async void onUpdateGoogleHomeName(object sender, RoutedEventArgs e)
        {
            await node.Config.SetGoogleHomeName(GoogleHomeNameTextBox.Text);
        }

        private async void onUpdateConnectionString(object sender, RoutedEventArgs e)
        {
            await node.Config.SetAzureConnectionString(connectionstringBox.Password);
        }

        private void onCancelScanWiFi(object sender, RoutedEventArgs e)
        {
            WiFiScanContentDialog.Hide();
        }

        private void onSSIDselected(object sender, RoutedEventArgs e)
        {
            SSIDtextBlock.Text = (string)FoundSSIDsListView.SelectedItem;
            WiFiScanContentDialog.Hide();
        }

        private async void onUpdateUsername(object sender, RoutedEventArgs e)
        {
            await node.Config.SetUsername(UsernameTextBox.Text);
        }

        private async void onNodeLocation(object sender, RoutedEventArgs e)
        {
            await node.Config.SetDeviceLocation(DeviceLocationTextBox.Text);
        }

        #endregion

        #region BLE node registration
        private string FormatToString(Windows.Storage.Streams.IBuffer value)
        {
            string formattedResult = string.Empty;
            CryptographicBuffer.CopyToByteArray(value, out byte[] data);
            if (data != null)
                formattedResult = Encoding.UTF8.GetString(data);
            return formattedResult;
        }

        private void UpdateConntionStatus(string status)
        {
            if (status[0] == '2')
            {
                WiFiStatus.Foreground = new SolidColorBrush(Colors.DarkGreen);
                WiFiStatus.Text = "\xEC3F";
            }
            else
            {
                WiFiStatus.Foreground = new SolidColorBrush(Colors.DarkRed);
                WiFiStatus.Text = "\xEB5E";
            }

            if (status[1] == '2')
                GoogleHomeStatus.Foreground = new SolidColorBrush(Colors.DarkGreen);
            else
                GoogleHomeStatus.Foreground = new SolidColorBrush(Colors.DarkRed);

            if (status[2] == '2')
                AzureStatus.Foreground = new SolidColorBrush(Colors.DarkGreen);
            else
                AzureStatus.Foreground = new SolidColorBrush(Colors.DarkRed);
        }

        private async void GattCharacteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            Debug.WriteLine($"Config Page > {SupportedUuids.TranslateUuid(sender.Uuid)} : {SupportedUuids.FormatToString(args.CharacteristicValue)}");
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                switch (sender.Uuid.ToString())
                {
                    case SupportedUuids.UUID_CON_WIFI_SCAN:
                        await Task.Delay(2000);
                        FoundSSIDs.Clear();
                        var ssids = (await node.Config.FetchSSIDs()).Split(',').ToList();
                        Debug.WriteLine($"Received {ssids.Count - 1} SSIDs ");
                        foreach (var ssid in ssids)
                        {
                            Debug.WriteLine(ssid);
                            if (!string.IsNullOrEmpty(ssid) && !FoundSSIDs.Contains(ssid))
                                FoundSSIDs.Add(ssid);
                        }
                        ScanningProgressRing.IsActive = false;
                        ScanningStatusTextBox.Text = "WiFi scan is completed.";
                        break;

                    case SupportedUuids.UUID_CON_DEVI_CONN:
                        var value = FormatToString(args.CharacteristicValue);
                        await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () =>
                        {
                            UpdateConntionStatus(value);
                        });

                        break;
                    default:
                        break;
                }

            });
        }

        #endregion
    }
}