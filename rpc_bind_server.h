#ifndef RPC_BIND_SERVER_H
#define RPC_BIND_SERVER_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "vxi_server.h"
#include "utilities.h"


class VXI_Server;         // forward declaration

class RPC_Bind_Server {

  public:

    // constructor and destructor

    RPC_Bind_Server ( VXI_Server & vs )
      : vxi_server(vs)
      {}

    ~RPC_Bind_Server ()
      { udp.stop(); tcp.stop(); };

    void      begin ();
    uint32_t  loop ();

  protected:

    uint32_t  process_data ( int len, bool onUDP );

    VXI_Server &    vxi_server;
    WiFiUDP         udp;
    WiFiServer_ext  tcp;

};

#endif