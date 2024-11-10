#ifndef TELNET_SERVER_H
#define TELNET_SERVER_H

/*!
  @file   telnet_server.h
  @brief  Declaration of the Telnet_Server class.
*/

#include "ESPTelnet.h"

extern ESPTelnet  Telnet;   ///< Global instance of ESPTelnet used by Telnet_Server

/*!
  @brief  The Telnet_Server class implements command checking
          and passthrough using the ESPTelnet Telnet service.
*/
class Telnet_Server {

  public:

    Telnet_Server ()  ///< Default constructor does nothing
      {}
    
    ~Telnet_Server () ///< Default destructor does nothing
      {}

    /*!
      @brief  Sets up the onTelnetInput callback and
              initiates the Telnet service.
    */
    void  begin ();

    /*!
      @brief  Call this at least once per main loop to
              process any passthrough data; also calls
              Telnet.loop()
    */
    void  loop ();

  protected:

    static  void onTelnetInput ( String s );

    static  bool  pass_through;   ///< State variable shows whether PASSTHROUGH is enabled
};

#endif