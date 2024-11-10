/*!
  @file   espBode.ino
  @brief  Main file of espBode on Arduino IDE
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "ESPTelnet.h"
#include "Streaming.h"
#include "wifi_config.h"
#include "credentials.h"
#include "debug.h"
#include "rpc_bind_server.h"
#include "vxi_server.h"
#include "telnet_server.h"
#include "awg_fy6900.h"

// global variables

AWG_FY6900      awg;                          ///< Use the FY6900 variant of the AWG_Server class
VXI_Server      vxi_server(awg);              ///< The VXI_Server
RPC_Bind_Server rpc_bind_server(vxi_server);  ///< The RPC_Bind_Server
Telnet_Server   telnet_server;                ///< The Telnet_Server

/*!
  @brief  Set up the WiFi connection.

  The WiFi setup is separated out from the rest of the setup()
  routine just to keep the latter from being too cluttered. Note
  that this function blocks until the WiFi connection is
  successfully established; in other words, the ESP-01 will
  be completely unresponsive if no WiFi connection is made.
*/
void setup_WiFi ()
{
  /*  The initial part of this function represents most of
      the code that has been carried over from the original
      espBode project with minimal modifications.
  */

  #if defined(STATIC_IP)
    IPAddress ip(ESP_IP);
    IPAddress mask(ESP_MASK);
    IPAddress gateway(ESP_GW);
    WiFi.config(ip, gateway, mask);
  #endif

  #if defined(WIFI_MODE_CLIENT)
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PSK);      // note: SSID and PSK are stored in credentials.h
  #elif defined(WIFI_MODE_AP)
    WiFi.softAP(WIFI_SSID, WIFI_PSK);     // note: SSID and PSK are stored in credentials.h
  #else
    #error PLEASE SELECT WIFI_MODE_AP OR WIFI_MODE_CLIENT!
  #endif

  while (WiFi.status() != WL_CONNECTED)
  {

    #ifdef USE_LED
      digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));    // LED will blink slowly while attempting to connect
    #endif

    if ( Debug.Channel() == DEBUG::VIA_SERIAL )   // Telnet is not yet available, so only use Debug if sent to Serial
    {
      Debug.Progress() << ". ";
    }

    delay(500);   // Wait 1/2 second before trying again
  }

  if ( Debug.Channel() == DEBUG::VIA_SERIAL )     // Telnet is still not yet available, so only use Debug if sent to Serial
  {
    Debug.Progress() << "\nWiFi connected; IP address = " << WiFi.localIP().toString() << "; MAC address = " << WiFi.macAddress() << "\n\n";
  }

  #ifdef USE_LED
    blink(5,100);               // blink the LED 5 times quickly to confirm connection
  #endif
}

/*!
  @brief  Standard Arduino setup() function to perform initializations.

  Here we need to set up the LED pin, initialize the serial port,
  connect to the WiFi, and initialize the various servers.
*/
void setup()
{
    Serial.begin(awg.baud_rate());

    #ifdef USE_LED
      pinMode(LED_BUILTIN, OUTPUT);
    #endif

  /*  Set desired debug level and output channel. Serial debugging
      at any level works flawlessly, but of course the Serial
      channel is needed to communicate with the AWG, so it is only
      suitable when testing without the AWG connected to the ESP-01.

      Telnet debugging allows testing WITH the AWG connected, but
      note that Telent with level = PACKET seems to get backed up,
      and the oscilloscope winds up having to retry several times
      to keep the communication flowing. It does work, but it slows
      everything down. However, debugging at level = PROGRESS or
      even level = SERIAL_IO works flawlessly. 
  */

  Debug.Via_Telnet();
  Debug.Filter_Progress();

  /*  Note that if Debug is directed to Telnet, anything sent to Debug at this point
      will effectively be lost since wifi is not yet connected. However, we do not want to
      assume that it is safe to direct the Debug messages to serial; if the AWG is connected,
      it may get confused. Therefore, we will direct the progress message below to serial
      only if Debug is specifically set to output to serial.
  */

  if ( Debug.Channel() == DEBUG::VIA_SERIAL )
  {
    Debug.Progress() << "Connecting to " << WIFI_SSID << " ";
  }

  /*  Initiate the WiFi connection. Note that this function
      will block until the connection is established.
  */

  setup_WiFi();

  /*  Initiailize the various servers - telnet_server,
      rpc_bind_server, and vxi_server.
  */

  awg.retry(2);               // validate settings with up to 2 retries
  vxi_server.begin();
  rpc_bind_server.begin();
  telnet_server.begin();
}

/*!
  @brief  Standard Arduino main loop

  The main loop simply calls the loop() method of each of the servers,
  allowing them to do any processing they need to do before passing
  control to the next server.
*/
void loop() {
  telnet_server.loop();
  rpc_bind_server.loop();
  vxi_server.loop();
}
