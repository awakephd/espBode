#include "rpc_bind_server.h"
#include "vxi_server.h"
#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"


/*** begin() ***********************************

  The following member function initializes the
  UDP and TCP servers to listen on the bind_port
  port.

*************************************************/

void RPC_Bind_Server::begin ()
{
//  vxi_server = vs;

  udp.begin(rpc::BIND_PORT);
  tcp.begin(rpc::BIND_PORT);

  Debug.Progress() << "Listening for RPC_BIND requests on UDP and TCP port " << rpc::BIND_PORT << "\n";
}

/*** loop() ******************************************

  The following member function should be called by
  the main loop of the program to process any UDP or
  TCP bind requests. It will call vxi_server.allocate()
  to request a vxi handler. If vxi_server can allocate
  and set up a handler, vxi_server.allocate will return
  the port on which the handler will listen; this can then
  be returned as part of the rpc_bind_response. If no
  vxi_handler is available, vxi_server.allocate will return
  zero, and an error message (out of resources) should be
  returned as part of the rpc_bind_response.

******************************************************/

uint32_t RPC_Bind_Server::loop ()
{
  int       len;
  uint32_t  rc = 0;

  /*  What to do if no vxi_server is available?

    There is no "out of resources" error code return from the
    RPC BIND request. We could respond with PROC_UNAVAIL, but
    that might suggest that the ESP simply cannot do RPC BIND
    at all (as opposed to not right now). Another optioon is to
    reject the message, but the enumerated reasons for rejection
    (RPC_MISMATCH, AUTH_ERROR) do not seem appropriate. For now,
    the solution is to ignore (not read) incoming requests until
    a vxi_server becomes available.

  */

  if ( vxi_server.available() ) {
    if ( udp.parsePacket() > 0 ) {
      Debug.Detail() << "\nUDP packet received on port " << rpc::BIND_PORT << "\n";

      len = get_packet(udp);

      rc = process_data(len, true);

      send_packet(udp,sizeof(bind_response_packet));

    } else {
      WiFiClient  tcp_client;

      tcp_client = tcp.accept();
    
      if ( tcp_client ) {
        Debug.Detail() << "\nTCP packet received on port " << rpc::BIND_PORT << "\n";

        len = get_packet(tcp_client);

        rc = process_data(len, false);

        send_packet(tcp_client,sizeof(bind_response_packet));
      }
    }
  }

  return rc;
}

uint32_t RPC_Bind_Server::process_data ( int len, bool onUDP )
{
  uint32_t  rc = rpc::SUCCESS;
  uint32_t  port;

  if ( rpc_request->program != rpc::PORTMAP ) {
    rc = rpc::PROG_UNAVAIL;

    Debug.Error() << "Invalid program (expected PORTMAP = 0x186A0; received ";
    Debug.printf("0x%08x)\n", (uint32_t)(rpc_request->program));

  } else if ( rpc_request->procedure != rpc::GET_PORT ) {
    rc = rpc::PROC_UNAVAIL;

    Debug.Error() << "Invalid procedure (expected GET_PORT = 3; received " << (uint32_t)(rpc_request->procedure) << ")\n";

  } else {

    Debug.Progress() << "\nPORTMAP command received on " << ( onUDP ? "UDP" : "TCP" ) << " port " << rpc::BIND_PORT << "\n";

    port = vxi_server.allocate();

    /*  The logic in the loop() routine should not allow
        the following condition to occur!  */

    if ( port == 0 ) {
      rc = rpc::GARBAGE_ARGS;   // not really the appropriate response, but we need to signal failure somehow!

      Debug.Error() << "PORTMAP failed: no vxi_handler available.\n";
    }
  }

  bind_response->rpc_status = rc;
  bind_response->vxi_port = port;

  return rc;
}
