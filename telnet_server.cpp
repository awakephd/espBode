/*!
  @file   telnet_server.cpp
  @brief  Definitions of Telnet_Server members and methods.
*/

#include "telnet_server.h"


ESPTelnet Telnet;     ///< Definition of global instance of ESPTelnet used by Telnet_Server


bool Telnet_Server::pass_through = false;


void Telnet_Server::begin ()
{
  Telnet.onInputReceived(onTelnetInput);  // connect incoming messages to the callback routine above
  Telnet.begin();
}


void Telnet_Server::loop ()
{
  //  Allow Telnet processing

  Telnet.loop();

  //  Copy data from the serial port if passthrough is enabled

  if ( pass_through ) {
    while ( Serial.available() > 0 ) {
      char c = Serial.read();

      Telnet << c;
    }
  }
}

/*!
  @brief  Callback function used by the ESPTelnet library.

  @param  input   Line of text received by the ESPTelnet library

  The Telnet library will call this callback function whenever a line of data (terminated
  with an \\n) is received. The callback function converts the string to upper case and trims
  leading/trailing whitespace. It then tests the string to see if it consists of a recognized
  command. If so, the command is processed and the results are reported back to the telnet client.

  Recognized commands:

    PASSTHROUGH - toggles the pass_through state

  If the string of data is not a recognized command, the callback function will either discard
  the string (if ! pass_through) or pass the string via the serial interface to the connected
  AWG (if pass_through).
*/
void Telnet_Server::onTelnetInput ( String input ) {

  String s(input); 
  s.trim();
  s.toUpperCase();

  if ( s == "PASSTHROUGH" ) {
    pass_through = ! pass_through;

    Telnet.flush();

    Telnet << "\n" << s << ( pass_through ? " ON" : " OFF" ) << "\n";

  } else if ( pass_through ) {
    Serial.println(input);
  }
}
