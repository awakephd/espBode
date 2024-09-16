#ifndef _ESP_CONFIG_H_
#define _ESP_CONFIG_H_

#include "ESPTelnet.h"
extern ESPTelnet telnet;

#define FY3200  1
#define FY6800  2
#define FY6900  3
#define JDS2800 4

/* Select the FY3200 or FY6900 or FY6800 type AWG*/
#define AWG FY6900

/* Select either AP or CLIENT mode:
    - AP - creates new network that oscilloscope can connect to
    - CLIENT - joins existing network
    */
//#define WIFI_MODE_AP
#define WIFI_MODE_CLIENT

/* WiFi credentials */
#define WIFI_SSID             "wlan_ssid"         // replace with SSID of the wifi network the oscilloscope is attached to
#define WIFI_PSK              "wlan_password"     // replace with the password for this network

/* Comment this for DHCP. However you'll need to obtain IP somehow. */
#define STATIC_IP

/* Static ip configuration */
#ifdef STATIC_IP
  #define ESP_IP              192,168,0,155       // Choose an IP address that is available on the network; for best results, reserve address in the router
  #define ESP_MASK            255,255,255,0
  #define ESP_GW              192,168,0,0         // Set the gateway as appropriate for the wifi network
#endif

#define ID                  "IDN-SGLT-PRI SDG1062X\n"   // ID used to simulate a Siglent AWG

#define RPC_PORT            (111)
#define LXI_PORT            (703)

#define PORTMAP             (0x000186A0)
#define VXI_11_CORE         (0x000607AF)

#define PORTMAP_GETPORT     (0x00000003)

#define RPC_SINGLE_FRAG     (0x80000000)
#define RPC_REPLY           (0x00000001)

#define VXI_11_CREATE_LINK  (10)
#define VXI_11_DESTROY_LINK (23)
#define VXI_11_DEV_WRITE    (11)
#define VXI_11_DEV_READ     (12)

#define RX_BUFF_SIZE        (128)

//#define DEBUG_PRINTS
#ifdef DEBUG_PRINTS
  #define DEBUG(TEXT)         Serial.println(TEXT);
#else
  #define DEBUG(TEXT)         telnet.println(TEXT);
#endif

#define LED_ON  LOW           // the ESP-O1 requires a LOW to turn on the built-in LED
#define LED_OFF HIGH

#endif /* _ESP_CONFIG_H_ */
