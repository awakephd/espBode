#ifndef ESPBODE_H
#define ESPBODE_H

/*  Comment this for DHCP. However, you'll
    need to obtain IP somehow. */

#define STATIC_IP

/* Static ip configuration */

#ifdef STATIC_IP
  #define ESP_IP              192,168,0,155       // Choose an IP address that is available on the network; for best results, reserve this address in the router
  #define ESP_MASK            255,255,255,0       // Typical mask for a local network
  #define ESP_GW              192,168,0,1         // Set the gateway as appropriate for the wifi network
#endif

/*  Select either AP or CLIENT mode:
    - AP - creates new network that oscilloscope can connect to
    - CLIENT - joins existing network */

//#define WIFI_MODE_AP
#define WIFI_MODE_CLIENT

/*  On the ESP-01 (not the -S version), the LED
    interferes with Serial, and therefore should
    not be used. Comment out the following line
    to prevent use of the LED.  */

#define USE_LED

/*  The ESP-01S uses LOW to turn on the LED and HIGH to turn it off  */

const int LED_ON = LOW;
const int LED_OFF = HIGH;

#endif