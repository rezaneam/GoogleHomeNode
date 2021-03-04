# Google Home Node

This project is introducing a proof of concept for a Smart Home Device/Sensor/Monitoring with different approach.
In this project Bosch enviroment sensors (BMP280, BME280, BME680) were utilized to monitor the temperature, humidity, pressure, and air-quality.

## What is innovative in this project

- The main IoT node configuration is very simple and you can update it anything.
- The main IoT node pushes realtime update via BluetoothLE to multiple connected devices.
- Agains many avaiable solutions you don't need to open port in your router/modem/firewall. that's really cumbersome and maybe risky.
- You don't need to deal with DNS and static IP address. that's a real pain.
- You don't need to register on expensive and complex services (Azure IoT Hub service has free plan).
- You can customize the whole project with a little effort.

## Components

This project includes following components

- ESP32:

It's the main IoT device which basicaly is an ESP32 module connected to an enviroment sensor. The module needs to connect to Internet via WiFi and connect to Azure services. This project is written in `Platform IO` so it should take care of everything (external libraries) if you use `Platform IO`.
You should be able to connect to the device and configure it via BluetoothLE and at anytime you can update the sensor configuration without any need to reset it.
WiFi name, Password, Azure Connection string, Google Home, device location (for example kitchen, livingroom) are configuration values needed to be set. Hopefully UWP and Xamarin applicaitons will help you to do it with any challenges.

- UWP Application:

This application is just for configuring the Google Home node. It's basically a `Windows 10` application and will show up in `Windows store` soon :)
By the way, you can run on Desktop, Windows Mobile, and `Windows IoT Core` (`Raspberry Pi`).

- Xamarin Application:

This project is an extended version of UWP Application for `Android` and `iOS` platform. Not ready yet.

- Azure:

This is an azure Function app if you want to use your own `Azure service`. contact me if you can't follow me.

## How set it up

- Google Home Node

First thing first. You need to build the Google Home Node device and program it.
I would recommand you first test it with a very basic ESP32 evaluation board and a BMP280/BME280 breakout board. This setup would cost you less than 5 USD. Of course having a `SSD1306` is a plus.
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

- Azure Function

- Azure IoT Hub

- IFTTT

You need to make a IFTTT account (it's free) and make a scenario called

## How it is working

The following figure shows the overal concept of the project.
<img src="assets/Diagram.png?raw=true" width="500px">

The Google Home Node (main ESP32 module) starts to connect to Azure IoT service (as set in the configuration) and tries to find the local `Google Home speaker` (namly Google Home, Google Home Mini, or any Google Home enable speaker). It of course drives the OLED and the sensor.
The Google Home Node is always in listening mode (at least for now) and doesn't send any information to `Azure IoT Hub`.

When you ask `Google Assistant` to give you the home temperature (or specific location/node), Google Assistant (not Google Home speaker) triggers an action in the IFTTT (which you have previously defined).

`IFTTT` triggers a webhook (better to a http request) which actually an Azure Function with a some inputs (username, keywords).
The receive information from Azure Function is sent to the Azure IoT and evantually received by the Google Home Node.

Google Home Node take actions if the receive information matches (username, location, etc). So Google Home Node asks the Google Home speaker (already linked) to say the temperature, humidity, ... values.
I used [esp8266 Google TTS](https://github.com/horihiro/esp8266-google-tts) to generate audio file and [esp8266 Google Home Notifier](https://github.com/horihiro/esp8266-google-home-notifier) to push the message/nofication to the Google Home Speaker.

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
