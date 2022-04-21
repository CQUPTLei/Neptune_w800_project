需要修改 net_params.h中的参数

#define PARAM_HOTSPOT_SSID "AP-SSID"  // your AP SSID
#define PARAM_HOTSPOT_PSK  "AP-KEY"  // your AP PSK

#define PARAM_HOTSPOT_TYPE WIFI_SEC_TYPE_PSK // defined in wifi_device_config.h

#define PARAM_SERVER_ADDR "TCP-SERVER-XXX.XXX.XXX.XXX" // your PC IP address
#define PARAM_SERVER_PORT 5678