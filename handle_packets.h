#ifndef HANDLE_PACKETS_H
#define HANDLE_PACKETS_H

#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "utilities.h"

extern cycle  port;

int handle_udp ( WiFiUDP udp );
int handle_lxi ( WiFiClient client );

#endif