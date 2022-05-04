
#ifndef WIFI_CONNECTER_H
#define WIFI_CONNECTER_H

#include "wifi_device.h"

#define PARAM_HOTSPOT_SSID "jaychouios"  // your AP SSID
#define PARAM_HOTSPOT_PSK  "12345679"  // your AP PSK

#define PARAM_HOTSPOT_TYPE WIFI_SEC_TYPE_PSK // defined in wifi_device_config.h

int ConnectToHotspot(WifiDeviceConfig* apConfig);

void DisconnectWithHotspot(int netId);

#endif  // WIFI_CONNECTER_H