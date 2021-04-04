#pragma once

struct ConnectionStatus
{
  bool isHomeConnected;
  bool isBLEAdvertising;
  bool isBLEConnected;
  bool isWiFiConnected;
  bool isInternetConnected;
  bool isCloudConnected;
};