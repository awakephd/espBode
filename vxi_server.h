#ifndef VXI_SERVER_H
#define VXI_SERVER_H

#include <ESP8266WiFi.h>
#include "wifi_ext.h"
#include "vxi_handler.h"
#include "utilities.h"


const int n_vxi_handlers = 1;


class VXI_Server {

  public:

    VXI_Server ()
      {}

    ~VXI_Server ()
      {}

    void      begin ();
    uint32_t  loop ();
    bool      available ();
    uint32_t  allocate ();

    static  void      set_timeout ( uint32_t milliseconds )
      { timeout = milliseconds; }

    static  uint32_t  get_timeout ()
      { return timeout; }

  protected:

    static cyclic_uint32  vxi_port;
    static uint32_t       timeout;
    
    VXI_Handler           vxi_handlers[n_vxi_handlers];
};


#endif