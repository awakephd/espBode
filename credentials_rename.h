#ifndef CREDENTIALS_H
#define CREDENTIALS_H

/*!
  @file   credentials_rename.h
  @brief  Edit the constants and rename to credentials.h.

  A file named credentials.h is required to provide the SSID
  and PSK for the wifi network to which the ESP-01 should connect.
  For security, the actual credentials.h file is omitted from the
  repository and the documentation. Howevever, this file can be
  edited and renamed to credentials.h to provide the necessary
  connection data.
*/

const char WIFI_SSID[] =  "MySSID";   ///< The SSID to which the ESP-01 should connect
const char WIFI_PSK[] =   "MyPSK";    ///< The password for the wifi network

#endif

