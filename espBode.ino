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

/*
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
*/

ESPTelnet       Telnet;
VXI_Server      vxi_server;
RPC_Bind_Server rpc_bind_server(vxi_server);
Telnet_Server   telnet_server;


/*** global variables **************/

Loop_State  loop_state;             // tracks the current state for the main loop
/*
WiFiUDP     udp;                    // used to receive BIND requests via UDP
WiFiServer  tcp_server(RPC_PORT);   // used to receive BIND requests via TCP
WiFiServer  vxi_server(0);          // used to process VXI commands via TCP
*/

/*** callback function for enhanced telnet communication ********************************

The telnet object will call this function whenever a line of data (terminated with an \n)
is received. The string will be converted to upper case and trimmed of any leading or
trailing whitespace. It will then be tested to see if it consists of the following command.
If so, the command is processed and the results are reported back to the telnet client.

  PASSTHROUGH - toggles the bPassThrough state

If the string of data is not a recognized command, the callback function will either
discard the string (if loop_state != ls_passthrough) or pass the string via the serial
interface to the connected AWG (if loop_state == ls_passthrough).

******************************************************************************************/
/*
void onTelnetInput ( String s ) {

  s.trim();
  s.toUpperCase();

  if ( s == "PASSTHROUGH" ) {
    loop_state = ( loop_state == ls_passthrough ) ? ls_ready_for_rpc : ls_passthrough;

    Telnet.flush();

    Telnet << "\n" << s << ( loop_state == ls_passthrough ? " ON" : " OFF" ) << "\n\n";

  } else if ( loop_state == ls_passthrough ) {
    Serial.println(s);
  }
}

*/
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

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // LED will blink slowly while attempting to connect

    if ( Debug.Channel() == DEBUG::VIA_SERIAL ) {
      Debug.Progress() << ". ";
    }

    delay(500);
  }

  if ( Debug.Channel() == DEBUG::VIA_SERIAL ) {
    Debug.Progress() << "\n\nWiFi connected; IP address = " << WiFi.localIP().toString() << "; MAC address = " << WiFi.macAddress() << "\n";
  }

  blink(5,100);               // blink the LED 5 times quickly to confirm connection
}


/*** Arduino setup() ************************************************

Here we need to set up the LED pin, initialize the serial port,
connect to the wifi, initialize the two wifi servers, and initialize
the telnet server.

*********************************************************************/

void setup() {
//  if(AWG == FY3200)
//    Serial.begin(9600);
//  else
    Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  // Set desired debug level and output channel

  Debug.Via_Serial();
  Debug.Level_Detail();

  /*  Note that if Debug is directed to Telnet, anything sent to Debug at this point
      will effectively be lost since wifi is not yet connected. However, we do not want to
      assume that it is safe to direct the Debug messages to serial; if the AWG is connected,
      it may get confused. Therefore, we will direct the progress message below to serial
      only if Debug is specifically set to output to serial. */

  if ( Debug.Channel() == DEBUG::VIA_SERIAL ) {
    Debug.Progress() << "Connecting to " << WIFI_SSID << " ";
  }

  /*  Initiate the WiFi connection. Note that this function
      will block until the connection is established.  */

  setup_WiFi();

  // Initialize telnet server

//  Telnet.onInputReceived(onTelnetInput);  // connect incoming messages to the callback routine above
//  Telnet.begin();

  // Initialize the wifi servers
/*
  udp.begin(RPC_PORT);
  tcp_server.begin();
  vxi_server.begin();
*/
  vxi_server.set_timeout(15000);     // if more than 2 seconds passes without any further packets, close the vxi_handler

  vxi_server.begin();
  rpc_bind_server.begin();
  telnet_server.begin();

  // Initialilze the initial "state" for the main loop

//  loop_state = ls_ready_for_rpc;
}


/*** Arduino main loop: loop() **************************************

The main loop will call Telnet.loop() to allow telnet processing
to occur. After that, its action is based on the loop_state variable,
Essentially, the loop waits for a valid BIND request to come in
via either UDP or TCP. Once the BIND request has succeeded, it
sets up vxi_server to listen on the next port in the queue, and
continues to process any VXI commands that it receives until it
receives the DESTROY_LINK command. At that point, it goes back to
listening for the next BIND request.

**********************************************************************/

void loop() {

  // give telnet a chance to work

//  Telnet.loop();

  //  What happens next depends on the loop_state

//  switch ( loop_state ) {
    
    /*  if loop_state == ls_passthrough, pass serial traffic to telnet;
        note that the passthrough state is turned on or off by a
        command sent to the telnet server.

    case ls_passthrough:
      while ( Serial.available() > 0 ) {
        char c = Serial.read();

        Telnet << c;
      }

      break;
*/
    /*  if loop_state == ls_ready_for_rpc, make preparations to listen for
        a BIND request on either UDP or TCP.  */
  /*      
    case ls_ready_for_rpc:
      Debug.Progress() << "\nListening for PORTMAP on UDP and TCP (Port:" << RPC_PORT << ")\n";

      loop_state = ls_wait_for_rpc;

      break;
*/
    /*  if loop_state == ls_wait_for_rpc, check for an rpc connection;
        if successful, change state to ls_ready_for_vxi; otherwise,
        keep waiting (loop back around).  */
/*
    case ls_wait_for_rpc:
      {
        int packetSize = udp.parsePacket();

        if (packetSize) {
          Debug.Progress() << "\nUDP packet received on port " << RPC_PORT << "\n";

          if ( process_packet(udp,PORTMAP,RPC_GETPORT) == 0 ) {
            loop_state = ls_ready_for_vxi;
          }

        } else {    // if no UDP packet, try TCP packet

          WiFiClient  tcp_client = tcp_server.accept();

          if ( tcp_client ) {
            Debug.Progress() << "\nTCP packet received on port " << RPC_PORT << "\n";

            if ( process_packet(tcp_client,PORTMAP,RPC_GETPORT) == 0 ) {
              loop_state = ls_ready_for_vxi;
            }
          }
        }
      }

      break;
*/
    /*  if loop_state == ls_ready_for_vxi, set up the vxi_server
        to listen on the next available port,  */
/*
    case ls_ready_for_vxi:
      Debug.Progress() << "Listening for VXI on port " << vxi_port() << "\n";

      loop_state = ls_wait_for_vxi;
  
      break;
*/
    /*  if loop_state == ls_wait_for_vxi, check for a vxi connection;
        if successful, process vxi commands until complete;
        otherwise, keep waiting (loop back around).  */
/*
    case ls_wait_for_vxi:
      {
        WiFiClient  vxi_client = vxi_server.accept();

        if ( vxi_client ) {
          vxi_client.setTimeout(1000);

          Debug.Progress() << "VXI connection established on port " << vxi_port() << "\n";

          while ( process_packet(vxi_client) == 0 ) {
            Telnet.loop();                                          // while we are processing commands, keep telnet working
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // toggle LED to give visual feedback
          }

          digitalWrite(LED_BUILTIN, LED_OFF);                       // when processing is complete, turn off LED

          loop_state = ls_ready_for_rpc;
        }
      }

      break;
*/
//    default:

//      break;
    
//  } // end switch

  telnet_server.loop();
  rpc_bind_server.loop();
  vxi_server.loop();
}
