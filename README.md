# Google Home Node

This project is introducing a proof of concept for a Smart Home Device/Sensor/Monitoring with different approach.
In this project Bosch enviroment sensors (BMP280, BME280, BME680) were utilized to monitor the temperature, humidity, pressure, and air-quality.

# Components

This project includes following components

- ESP32
  It's the main IoT device which basicaly is an ESP32 module connected to an enviroment sensor. The module needs to connect to Internet via WiFi and connect to Azure services. This project is written in Platform IO so it should take care of everything (external libraries) if you use Platform IO.
  You should be able to connect to the device and configure it via BluetoothLE and at anytime you can update the sensor configuration without any need to reset it.
  WiFi name, Password, Azure Connection string, Google Home, device location (for example kitchen, livingroom) are configuration values needed to be set. Hopefully UWP and Xamarin applicaitons will help you to do it with any challenges.

- UWP Application
  This application is just for configuring the Google Home node. It's basically a Windows 10 application and will show up in Windows store soon :)
  By the way, you can run on Desktop, Windows Mobile, and Windows IoT Core (Raspberry Pi).

- Xamarin Application
  This project is an extended version of UWP Application for Android and iOS platform. Not ready yet.

- Azure
  This is an azure Function app if you want to use your own Azure service. contact me if you can't follow me.

# How it is working

The following figure shows the overal concept of the project.
<img src="assets/Diagram.png?raw=true" width="500px">

# How set it up

# Platform/IDE(s)

- [Platform IO](https://platformio.org/)
- [Arduino](https://www.arduino.cc/)
- [Espressif ESP-32](https://www.espressif.com/en/products/socs/esp32)

# Supported Sensors

- [Bosch BMP280](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)
- [Bosch BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)

# Dependent Libraries

- [Azure IoT Arduino](https://github.com/Azure/azure-iot-arduino)
- [Azure IoT Protocol HTTP](https://github.com/Azure/azure-iot-arduino-protocol-http)
- [Azure IoT Protocol MQTT](https://github.com/Azure/azure-iot-arduino-protocol-mqtt)
- [Azure IoT Utility](https://github.com/Azure/azure-iot-arduino-utility)
- [NimBLE Arduino](https://github.com/h2zero/NimBLE-Arduino) - Lite version of the BluetoothLE to save some RAM
- [esp8266 Google Home Notifier](https://github.com/horihiro/esp8266-google-home-notifier)
- [esp8266 Google TTS](https://github.com/horihiro/esp8266-google-tts)
- [Adafruit BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library) - Locally/Modified in the project
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - Extensively modified and locally imported.

# Future Improvment

- Bosch BME680
- Improve the UWP Application layout and code
- Deploy App in the Windows Store
- Support for Android & iOS platforms
- Support for User/location
- Multi-language support
- Find a way to integrate with Google Assistant
