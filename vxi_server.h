#ifndef VXI_SERVER_H
#define VXI_SERVER_H

#include <ESP8266WiFi.h>
#include "wifi_ext.h"
#include "utilities.h"
#include "awg_server.h"


class VXI_Server {

  public:
  
    enum Read_Type {
      rt_none           = 0,
      rt_identification = 1,
      rt_parameters     = 2
    };

  public:

    VXI_Server ( AWG_Server & awg );

    ~VXI_Server ();

    void      loop ();

    void      begin ( bool bNext = false );

    void      begin_next ()
      { begin(true); }

    bool      available ()
      { return ( ! client ); }

    uint32_t  allocate ();

    uint32_t  port ()
      { return vxi_port; }

  protected:

    void  create_link ();
    void  destroy_link ();
    void  read ();
    void  write ();
    bool  handle_packet ();
    void  parse_scpi ( char * buffer );
    void  process_parameters ( char * parameter_context );
    int   get_id ( const char * id_text, const char * const id_list[], size_t id_cnt );

    WiFiServer_ext  tcp_server;
    WiFiClient      client;
    Read_Type       read_type;
    uint32_t        rw_channel;
    cyclic_uint32_t vxi_port;
    AWG_Server &    awg_server;    
};


#endif