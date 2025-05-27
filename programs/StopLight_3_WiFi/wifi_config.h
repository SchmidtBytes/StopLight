#ifndef STOP_LIGHT_WIFI_CONFIG_H
#define STOP_LIGHT_WIFI_CONFIG_H

#include <IPAddress.h>

// Enable Access Point and client mode
#define WIFI_ENABLE_AP
#define WIFI_ENABLE_CLIENT

// Client mode credentials
const char *ssid        = "<WiFi_SSID>";
const char *password    = "<WiFi_Password>";

// Access Point mode credentials
const char *ssid_ap     = "<WiFi_AP_SSID>";
const char *password_ap = "<WiFi_AP_Password>";

// Access Point IP config
const IPAddress ap_ip(192, 168, 4, 1);
const IPAddress ap_mask(255, 255, 255, 0);
const IPAddress ap_leaseStart(192, 168, 4, 2);
const IPAddress ap_dns(8, 8, 4, 4);

// web message state definitions
#define MSG_OFF          0 // Do not show a message
#define MSG_STATE_UPDATE 1 // Show state update message
#define MSG_SLEEP_UPDATE 2 // Show sleep update message
#define MSG_UNKNOWN_CMD  3 // Show unknown command message
#define MSG_ERROR        9 // Show error state when none of the above apply

#endif // STOP_LIGHT_WIFI_CONFIG_H