#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

/*!
  @file   wifi_config.h
  @brief  Configuration details for the wifi connection.
*/

/*!
  @brief  Enable static IP address configuration.

  Comment out this define to use DHCP. However, you will
  need to obtain the IP address via the router or other means.
*/
#define STATIC_IP

/* Static ip configuration */

#ifdef STATIC_IP
  #define ESP_IP              192,168,0,155       ///< Choose an IP address that is available on the network; for best results, reserve this address in the router
  #define ESP_MASK            255,255,255,0       ///< Typical mask for a local network
  #define ESP_GW              192,168,0,1         ///< Set the gateway as appropriate for the wifi network
#endif

/*!
  @brief  Select either AP or CLIENT mode.

  AP - creates a new network that the oscilloscope can connect to\n
  CLIENT - joins existing network
*/
#define WIFI_MODE_CLIENT
//#define WIFI_MODE_AP

#endif