using ConfigTool.BLE;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Security.Cryptography;
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
    public sealed partial class SensorPage : Page
    {
        Node node;
        public SensorPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter == null) return;
            if ((Node)e.Parameter == null) return;
            node = (Node)e.Parameter;
            node.SetEvent(GattCharacteristic_ValueChanged);
            float value;
            if (float.TryParse(node.Sensors.Temperature, out value))
                TempGauge.Value = value;

            if (float.TryParse(node.Sensors.Humidity, out value))
            {
                HumidityStackPanel.Visibility = Visibility.Visible;
                HumidityGauge.Value = value;
            }
            else
                HumidityStackPanel.Visibility = Visibility.Collapsed;
                

            if (float.TryParse(node.Sensors.Pressure, out value))
            {
                PressureStackPanel.Visibility = Visibility.Visible;
                PressureGauge.Value = value / 100;
            }
            else
                PressureStackPanel.Visibility = Visibility.Collapsed;
            
            if (float.TryParse(node.Sensors.AirQualityIndex, out value))
            {
                AirQualityStackPanel.Visibility = Visibility.Visible;
                AirQualityGauge.Value = value;
            }
            else
                AirQualityStackPanel.Visibility = Visibility.Collapsed;
               
          

        }

        private string FormatToString(Windows.Storage.Streams.IBuffer value)
        {
            string formattedResult = string.Empty;
            CryptographicBuffer.CopyToByteArray(value, out byte[] data);
            if (data != null)
                formattedResult = Encoding.UTF8.GetString(data);
            return formattedResult;
        }


        private async void GattCharacteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            Debug.WriteLine($"Sensor Page > {SupportedUuids.TranslateUuid(sender.Uuid)} : {SupportedUuids.FormatToString(args.CharacteristicValue)}");
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                float value;
                switch (sender.Uuid.ToString())
                {
                    case SupportedUuids.UUID_SEN_TMP:
                        if (float.TryParse(FormatToString(args.CharacteristicValue), out value))
                            TempGauge.Value = value;
                        break;

                    case SupportedUuids.UUID_SEN_HUM:
                        if (float.TryParse(FormatToString(args.CharacteristicValue), out value))
                            HumidityGauge.Value = value;
                        break;

                    case SupportedUuids.UUID_SEN_BAR:
                        if (float.TryParse(FormatToString(args.CharacteristicValue), out value))
                            PressureGauge.Value = value / 100;
                        break;
                    default:
                        break;
                }

            });
        }

    }
}
