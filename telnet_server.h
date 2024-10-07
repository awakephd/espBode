#ifndef TELNET_SERVER_H
#define TELNET_SERVER_H

#include "ESPTelnet.h"


extern ESPTelnet  Telnet;


class Telnet_Server {

  public:

    Telnet_Server ()
      {}
    
    ~Telnet_Server ()
      {}

    void  begin ();
    void  loop ();

  protected:

    static  void onTelnetInput ( String s );

    static  bool  bPassThrough;
};


#endif