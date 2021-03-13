# Google Home Node

This project introduces proof of concept for a Smart Home Device/Sensor/Monitoring with a different approach.
In this project, Bosch environment sensors (BMP280, BME280, BME680) were utilized to monitor the temperature, humidity, pressure, and air quality.

## What is innovative in this project

- The main IoT node configuration is straightforward and you can update it anything.
- The main IoT node pushes real-time updates via BluetoothLE to multiple connected devices.
- Against many available solutions, you don't need to open a port in your router/modem/firewall. That's really cumbersome and may be risky.
- You don't need to deal with DNS and static IP address. that's a real pain.
- You don't need to register on expensive and complicated services (Azure IoT Hub service has a free plan).
- You can customize the whole project with a little effort.

## Components

This project includes the following components

- ESP32:

It's the main IoT device which basically is an ESP32 module connected to an environment sensor. The module needs to connect to the Internet via WiFi and connect to Azure services. This project is written in `Platform IO` so it should take care of everything (external libraries) if you use `Platform IO`.
You should be able to connect to the device and configure it via BluetoothLE and at any time, you can update the sensor configuration without any need to reset it.
WiFi name, Password, Azure Connection string, Google Home, device location (for example, kitchen, living room) are configuration values needed to be set. Hopefully, UWP and Xamarin applications will help you to do it with any challenges.

- UWP Application:

This application is just for configuring the Google Home node. It's basically a `Windows 10` application and will show up in `Windows store` soon :)
By the way, you can run on Desktop, Windows Mobile, and `Windows IoT Core` (`Raspberry Pi`).

- ~~Xamarin Application~~: (not ready yet)

This project is an extended version of UWP Application for `Android` and `iOS` platform. Not ready yet.

- Azure:

This is an Azure Function app if you want to use your own `Azure service`. contact me if you can't follow me.

## How set it up

- Google Home Node

First thing first, You need to build the Google Home Node device and program it.
I would recommend you first test it with a very basic ESP32 evaluation board and a BMP280/BME280 breakout board. This setup would cost you less than 5 USD. Of course, having a `SSD1306` is a plus.
Wire the sensor as follows and flash the board via Platform IO in `Visual Studio Code`.

SDA <-> ESP32 Pin5

SCL <-> ESP32 Pin4

VCC <-> ESP32 3.3v

GND <-> ESP32 GND

You can also use the PCB design to make your custom compact module with more functionality and battery support.
Find the attached 3.5x3.5 cm [PCB design](/KiCad-Design/KiCad-Design.kicad_pcb).

PCB layout

<img src="assets/KiCad-Design-Front.jpg?raw=true" width="500px">

<img src="assets/KiCad-Design-Back.jpg?raw=true" width="500px">

- Azure IoT Hub

After creating the `Azure` account the first step is to make a new `IoT Hub` service in your Azure account. To do so just type in IoT Hub and create a new account (you pick F1 which is entirely free). Then Navigate to the IoT Hub service you created and make an IoT Device.
Click on the IoT Device that you have just created and copy the connection string. You need to set this connection string in your Google Home node.
To test if this part is working properly just send a direct method with the following payload and check the logs in your serial port.
So, now you are done with this part.

- Azure Function
In the same way make an Azure Function service in your Azure account.
- IFTTT

You need to make an IFTTT account (it's free) and make a scenario called.
Try to create an applet in IFTTT, add `Google Assistant` as a trigger source. Remember to pick _say a phrase with a text ingredient_
Type in your preferred keywords followed by $ sign and provide a response for that.
Then pick `Webhooks` (web request) as the service. So copy and paste the Azure Function URL in URL field. You pick either Get or Post request and for the content type choose JSON. paste the following structure there.

{
  "Action": "Google Home",
  "Language": "en",
  "UserID": "YourUserID",
  "Key": "TextField"
}

## How it is working

The following figure shows the overal concept of the project.
<p align="center">
  <img width="460" width="500px" src="assets/Diagram.png">
</p>

The Google Home Node (main ESP32 module) starts to connect to Azure IoT service (as set in the configuration) and tries to find the local `Google Home speaker` (namely Google Home, Google Home Mini, or any Google Home enable speaker). It also drives the OLED and the sensor.
The Google Home Node is always in listening mode (at least for now) and doesn't send any information to `Azure IoT Hub`.

When you ask `Google Assistant` to give you the home temperature (or specific location/node), Google Assistant (not Google Home speaker) triggers an action in the IFTTT (which you have previously defined).

`IFTTT` triggers a webhook (better to an HTTP request) which actually an Azure Function with some inputs (username, keywords).
The received information from Azure Function is sent to the Azure IoT and eventually received by the Google Home Node.

Google Home Node would take action if the received information matches (username, location, etc.). So Google Home Node asks the Google Home speaker (already linked) to say the temperature, humidity, ... values.
I used [esp8266 Google TTS](https://github.com/horihiro/esp8266-google-tts) to generate an audio file and [esp8266 Google Home Notifier](https://github.com/horihiro/esp8266-google-home-notifier) to push the message/notification to the Google Home Speaker.

## Platform/IDE(s)

- [Platform IO](https://platformio.org/)
- [Arduino](https://www.arduino.cc/)
- [Espressif ESP-32](https://www.espressif.com/en/products/socs/esp32)

## Supported Sensors

- [Bosch BMP280](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)
- [Bosch BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)

## Dependent Libraries

- [Azure IoT Arduino](https://github.com/Azure/azure-iot-arduino)
- [Azure IoT Protocol HTTP](https://github.com/Azure/azure-iot-arduino-protocol-http)
- [Azure IoT Protocol MQTT](https://github.com/Azure/azure-iot-arduino-protocol-mqtt)
- [Azure IoT Utility](https://github.com/Azure/azure-iot-arduino-utility)
- [NimBLE Arduino](https://github.com/h2zero/NimBLE-Arduino) - Lite version of the BluetoothLE to save some RAM
- [esp8266 Google Home Notifier](https://github.com/horihiro/esp8266-google-home-notifier)
- [esp8266 Google TTS](https://github.com/horihiro/esp8266-google-tts)
- [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library) - Locally/Modified in the project
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - Extensively modified and locally imported.

## Future Improvment

- Bosch BME680
- Improve the UWP Application layout and code
- Deploy App in the Windows Store
- Support for Android & iOS platforms
- Support for User/location
- Multi-language support
- Find a way to integrate with Google Assistant
- Connect to [HASS](https://www.home-assistant.io/)
