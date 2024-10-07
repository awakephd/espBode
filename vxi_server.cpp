#include "vxi_server.h"
#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"


void VXI_Server::begin ()
{
  // nothing to do for now, but in the future may allocate vxi_handlers dynamically here
}


uint32_t VXI_Server::loop ()
{
  for ( int i = 0; i < n_vxi_handlers; i++ ) {
    vxi_handlers[i].process();
  }

  return 0;
}


bool VXI_Server::available ()
{
  bool  b = false;

  for ( int i = 0; !b && i < n_vxi_handlers; i++ ) {
    b = ( vxi_handlers[i].port() == 0 );
  }

  return b;
}


uint32_t VXI_Server::allocate ()
{
  uint32_t port = 0;

  for ( int i = 0; port == 0 && i < n_vxi_handlers; i++ ) {
    if ( vxi_handlers[i].port() == 0 ) {
      port = vxi_port++;

      vxi_handlers[i].begin(port);
    }
  }

  return port;
}

uint32_t        VXI_Server::timeout;
cyclic_uint32   VXI_Server::vxi_port(rpc::VXI_PORT_START, rpc::VXI_PORT_END);
