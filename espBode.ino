#include <ESP8266WiFi.h>
#include "esp_network.h"
#include "esp_config.h"
#include "ESPTelnet.h"

//#include "dump.h"
//#include "utilities.h"

#ifdef USE_UDP
  #include <WiFiUdp.h>
  #include <iostream>
//  #include <string>
  #include <sstream>
  #include "handle_packets.h"
#endif

#if AWG == FY3200
  #include "esp_fy3200.h"
#elif AWG == FY6800
  #include "esp_fy6800.h"
#elif AWG == FY6900
  #include "esp_fy6900.h"
#elif AWG == JDS2800
  #include "jds2800.h"
#else
  #error "Please select an AWG in esp_config.h"
#endif

#ifdef USE_UDP
  WiFiUDP udp;
#else
  WiFiServer rpc_server(RPC_PORT);
#endif

WiFiServer lxi_server(0);
ESPTelnet telnet;


/*** Global variables and callback function for enhanced telnet communication ***

The telnet object will call this function whenever a line of data (terminated with an \n)
is received. The string will be converted to upper case and trimmed of any leading or
trailing whitespace. It will then be tested to see if it consists of the following command.
If so, the command is processed and the results are reported back to the telnet client.

  PASSTHROUGH - toggles the bPassThrough state

If the string of data is not a recognized command, the callback function will either
discard the string (if loop_state != ls_passthrough) or pass the string via the serial
interface to the connected AWG (if loop_state == ls_passthrough).

***********************************************************************************/

enum {
  ls_passthrough = 1,
  ls_wait_for_rpc = 2,
  ls_wait_for_lxi = 4
} loop_state;

void onTelnetInput ( String s ) {

  s.trim();
  s.toUpperCase();

  if ( s == "PASSTHROUGH" ) {
    loop_state = ( loop_state == ls_passthrough ) ? ls_wait_for_rpc : ls_passthrough;

    s = s + ( loop_state == ls_passthrough ? " ON" : " OFF" );

    telnet.println(s);

  } else if ( loop_state == ls_passthrough ) {
    Serial.println(s);
  }
}

/*** blink() ****************************************************

A small utility function to allow visual user feedback by
blinking the built-in LED a selected number of times at a
selected interval (specified in milliseconds).

*****************************************************************/

void blink ( int count, int interval )
{
    for ( int i = 0; i < count; i++ ) {
      digitalWrite(LED_BUILTIN, LED_ON);
      delay(interval);
      digitalWrite(LED_BUILTIN, LED_OFF);
      delay(interval);
    }
}

/*** Arduino setup() *********************************************

Here we need to set up the LED pin, initialize the serial port,
connect to the wifi, initialize the two wifi servers, and initialize
the telnet server.

*****************************************************************/

void setup() {
    if(AWG == FY3200)
      Serial.begin(9600);
    else
      Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);

    // We start by connecting to a WiFi network

    /* Note that if DEBUG is directed to telnet, it will effectively be lost at this
       point since wifi is not yet connected. However, we do not want to assume that
       it is safe to direct the DEBUG messages to serial; if the AWG is connected, it
       may get confused. Direct these message to serial only if DEBUG is specifically
       set to output to serial. */

    DEBUG("Connecting to ");      
    DEBUG(WIFI_SSID);             
                                  
#if defined(STATIC_IP)
    IPAddress ip(ESP_IP);
    IPAddress mask(ESP_MASK);
    IPAddress gateway(ESP_GW);
    WiFi.config(ip, gateway, mask);
#endif

#if defined(WIFI_MODE_CLIENT)
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PSK);
#elif defined(WIFI_MODE_AP)
    WiFi.softAP(WIFI_SSID, WIFI_PSK);
#else
    #error PLEASE SELECT WIFI_MODE_AP OR WIFI_MODE_CLIENT!
#endif

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // LED will blink slowly while attempting to connect
    }

    blink(5,100);               // blink the LED 5 times quickly to confirm connection

    DEBUG("WiFi connected");
    DEBUG("IP address: ");
    DEBUG(WiFi.localIP().toString());

    // Initialilze the "state" in which the loop() will operate

    loop_state = ls_wait_for_rpc;

    // Initialize telnet server

    telnet.onInputReceived(onTelnetInput);  // connect incoming messages to the callback routine above
    telnet.begin();

    // Initialize the wifi servers

#ifdef USE_UDP
    udp.begin(RPC_PORT);
#else
    rpc_server.begin();
#endif

    lxi_server.begin();
}

void loop() {

    // give telnet a chance to work

    telnet.loop();

    /* What happens next depends on the "state":

       if state = ls_passthrough, pass serial traffic to telnet;
          note that the passthrough state is turned on or off by
          a command sent to the telnet server.
       if state = ls_wait_for_rpc, check for an rpc connection;
          if successful, change state to ls_wait_for_lxi;
          otherwise, keep waiting (loop back around).
       if state = ls_wait_for_lxi, check for an lxi connection;
          if successful, process lxi commands until complete;
          otherwise, keep waiting (loop back around).
    */

    switch ( loop_state ) {

      case ls_passthrough:
        while ( Serial.available() > 0 ) {
          int c = Serial.read();
          String s{char(c)};

          telnet.print(s);
        }

        break;

      case ls_wait_for_rpc:
        {
#ifdef USE_UDP
          int packetSize = udp.parsePacket();

          if (packetSize) {
            // receive incoming UDP packets
            std::stringstream s;

            s << "Received " << packetSize << " bytes from " << udp.remoteIP().toString().c_str() << ":" << udp.remotePort();

            DEBUG(s.str().c_str());
/*
            int len = udp.read(udp_buffer, UDP_BUFFER_SIZE);

            s = std::stringstream();

            s << "Length = " << len;

            DEBUG(s.str().c_str());

            telnet.loop();

            if (len > 0) {
              DEBUG(dump(udp_buffer,len).c_str());
            }
*/
            if ( handle_udp(udp) == 0 )
              loop_state = ls_wait_for_lxi;
              lxi_server.stop();

              s = std::stringstream();
              s << "Listening on port = " << uint32_t(port) << "(" << std::hex << uint32_t(port) << ")" << std::endl;

              DEBUG(s.str().c_str());

              lxi_server = WiFiServer(port++);
              lxi_server.begin();
          }
#else
          WiFiClient  rpc_client = rpc_server.accept();

          if ( rpc_client ) {
            DEBUG("RPC CONNECTION");
            handlePacket(rpc_client);
            loop_state = ls_wait_for_lxi;
          }
#endif
        }

        break;

      case ls_wait_for_lxi:
        {
//          DEBUG("Waiting for LXI connection");

          WiFiClient  lxi_client = lxi_server.accept();

          if ( lxi_client ) {
            lxi_client.setTimeout(1000);
            DEBUG("LXI CONNECTION.");

            while ( handlePacket(lxi_client) == 0 ) {
              telnet.loop();                                          // while we are processing commands, keep telnet working
              digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // toggle LED to give visual feedback
            }

            digitalWrite(LED_BUILTIN, LED_OFF);                       // when processing is complete, turn off LED

            loop_state = ls_wait_for_rpc;
//          } else {
//            DEBUG("LXI server not accepted");
          }
        }

        break;

      default:

        break;

    } // end switch
}
