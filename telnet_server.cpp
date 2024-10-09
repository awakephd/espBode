#include "telnet_server.h"


bool Telnet_Server::bPassThrough = false;


ESPTelnet Telnet;


void Telnet_Server::begin ()
{
   Telnet.onInputReceived(onTelnetInput);  // connect incoming messages to the callback routine above
  Telnet.begin();
}


void Telnet_Server::loop ()
{
  //  Allow Telnet processing

  Telnet.loop();

  if ( bPassThrough ) {
    while ( Serial.available() > 0 ) {
      char c = Serial.read();

      Telnet << c;
    }
  }
}

/*** callback function for enhanced telnet communication ********************************

The telnet object will call this function whenever a line of data (terminated with an \n)
is received. The string will be converted to upper case and trimmed of any leading or
trailing whitespace. It will then be tested to see if it consists of the following command.
If so, the command is processed and the results are reported back to the telnet client.

  PASSTHROUGH - toggles the bPassThrough state

If the string of data is not a recognized command, the callback function will either
discard the string (if ! bPassThrough) or pass the string via the serial
interface to the connected AWG (if bPassThrough).

******************************************************************************************/

void Telnet_Server::onTelnetInput ( String input ) {

  String s(input); 
  s.trim();
  s.toUpperCase();

  if ( s == "PASSTHROUGH" ) {
    bPassThrough = ! bPassThrough;

    Telnet.flush();

    Telnet << "\n" << s << ( bPassThrough ? " ON" : " OFF" ) << "\n";

  } else if ( bPassThrough ) {
    Serial.println(input);
  }
}

