#ifndef VXI_HANDLER_H
#define VXI_HANDLER_H

#include <ESP8266WiFi.h>
#include "wifi_ext.h"


class VXI_Handler {

  enum Read_Type {
    rt_none           = 0,
    rt_identification = 1,
    rt_parameters     = 2
  };

  public:

    VXI_Handler ()
      {}

    ~VXI_Handler ()
      {}

    void  begin ( uint32_t port );
    void  process ();

    uint32_t  port ()
      { return vxi_port; }

  protected:

    void  create_link ( WiFiClient & client );
    void  destroy_link ( WiFiClient & client );
    void  read ( WiFiClient & client );
    void  write ( WiFiClient & client );
    bool  handle_packet ( WiFiClient & client );
    void  parse_scpi ( char * buffer );
    void  process_parameters ( char * parameter_context );
    int   get_id ( const char * id_text, const char * id_list[], size_t id_cnt );

    WiFiServer_ext  tcp_server;
    WiFiClient      client;
    uint32_t        vxi_port;
    uint32_t        last_packet_time;
    Read_Type       read_type;
    uint32_t        rw_channel;
};


#endif