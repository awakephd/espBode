#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "ESPTelnet.h"
#include "Streaming.h"
#include "espBode.h"
#include "credentials.h"
#include "debug.h"
#include "rpc_bind_server.h"
#include "vxi_server.h"
#include "telnet_server.h"
#include "awg_fy6900.h"


/*** global variables **************/

AWG_FY6900      awg(2);             // 2 retries if get value != set value
VXI_Server      vxi_server(awg);
RPC_Bind_Server rpc_bind_server(vxi_server);
Telnet_Server   telnet_server;


/*** WiFi setup *********************************************

The WiFi setup is separated out from the rest of the setup()
routine just to keep the latter from being too cluttered. Note
that this function blocks until the WiFi connection is
successfully established; in other words, the ESP-01 will
be completely unresponsive if no WiFi connection is made.

**************************************************************/

void setup_WiFi ()
{
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

    if ( Debug.Channel() == DEBUG::VIA_SERIAL )
    {
      Debug.Progress() << ". ";
    }

    delay(500);
  }

  if ( Debug.Channel() == DEBUG::VIA_SERIAL )
  {
    Debug.Progress() << "\nWiFi connected; IP address = " << WiFi.localIP().toString() << "; MAC address = " << WiFi.macAddress() << "\n\n";
  }

#ifdef USE_LED
  blink(5,100);               // blink the LED 5 times quickly to confirm connection
#endif

}


/*** Arduino setup() ************************************************

Here we need to set up the LED pin, initialize the serial port,
connect to the wifi, initialize the two wifi servers, and initialize
the telnet server.

*********************************************************************/

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
      even level = SERIAL_IO works flawlessly.  */

  Debug.Via_Telnet();
  Debug.Level_Progress();

  /*  Note that if Debug is directed to Telnet, anything sent to Debug at this point
      will effectively be lost since wifi is not yet connected. However, we do not want to
      assume that it is safe to direct the Debug messages to serial; if the AWG is connected,
      it may get confused. Therefore, we will direct the progress message below to serial
      only if Debug is specifically set to output to serial. */

  if ( Debug.Channel() == DEBUG::VIA_SERIAL )
  {
    Debug.Progress() << "Connecting to " << WIFI_SSID << " ";
  }

  /*  Initiate the WiFi connection. Note that this function
      will block until the connection is established.  */

  setup_WiFi();

  /*  Initiailize the various servers - telnet_server,
      rpc_bind_server, and vxi_server.  */

  vxi_server.begin();
  rpc_bind_server.begin();
  telnet_server.begin();
}


/*** Arduino main loop: loop() **************************************

The main loop calls telnet_server.loop() to enable telnet processing
and to process PASSTHROUGH commands. It calls rpc_bind_server.loop()
to process any PORTMAP commands. It calls vxi_server.loop() to process
any current VXI commands. Then it goes back and does it all again!

**********************************************************************/

void loop() {
  telnet_server.loop();
  rpc_bind_server.loop();
  vxi_server.loop();
}
