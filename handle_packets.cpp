#include "ESPTelnet.h"
#include <sstream>
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "utilities.h"
#include "dump.h"

#define PACKET_BUFFER_SIZE   1024

cycle port(9010,9019);

const uint32_t  id_get_port = 3;
const uint32_t  id_vxi_core = 395183;

uint8_t  packet_buffer[PACKET_BUFFER_SIZE];

extern  ESPTelnet telnet;

class udp_request {

  public:

    big_end_uint32   xid;
    big_end_uint32   msg_type;
    big_end_uint32   rpc_version;
    big_end_uint32   program;
    big_end_uint32   program_version;
    big_end_uint32   procedure;
    big_end_uint32   credentials_l;
    big_end_uint32   credentials_h;
    big_end_uint32   verifier_l;
    big_end_uint32   verifier_h;
    big_end_uint32   getport_program;
    big_end_uint32   getport_version;
    big_end_uint32   getport_protocol;
    big_end_uint32   getport_port;

};

class udp_response {

  public:

    big_end_uint32   xid;
    big_end_uint32   msg_type;
    big_end_uint32   reply_state;
    big_end_uint32   verifier_l;
    big_end_uint32   verifier_h;
    big_end_uint32   rpc_status;
    big_end_uint32   vxi_port;

};

class lxi_request_rw {

  public:

    big_end_uint32  xid;
    big_end_uint32  msg_type;
    big_end_uint32  rpc_version;
    big_end_uint32  program;
    big_end_uint32  program_version;
    big_end_uint32  procedure;
    big_end_uint32  credentials_l;
    big_end_uint32  credentials_h;
    big_end_uint32  verifier_l;
    big_end_uint32  verifier_h;
    big_end_uint32  link_id;
    big_end_uint32  io_timeout;
    big_end_uint32  lock_timeout;
    big_end_uint32  flags;
    big_end_uint32  data_len;
    uint8_t         data[];

};

class lxi_request_link {

  public:

    big_end_uint32  frag;
    big_end_uint32  xid;
    big_end_uint32  msg_type;
    big_end_uint32  rpc_version;
    big_end_uint32  program;
    big_end_uint32  program_version;
    big_end_uint32  procedure;
    big_end_uint32  credentials_l;
    big_end_uint32  credentials_h;
    big_end_uint32  verifier_l;
    big_end_uint32  verifier_h;
    big_end_uint32  link_id;
    big_end_uint32  io_timeout;
    big_end_uint32  lock_timeout;
    big_end_uint32  flags;
    big_end_uint32  data_len;
    uint8_t         data[];

};

class lxi_response_rw {

  public:

    big_end_uint32  frag;
    big_end_uint32  xid;
    big_end_uint32  msg_type;
    big_end_uint32  reply_state;
    big_end_uint32  verifier_l;
    big_end_uint32  verifier_h;
    big_end_uint32  rpc_status;
    big_end_uint32  error_code;
    big_end_uint32  reason;
    big_end_uint32  data_len;
    uint8_t         data[];

};

class lxi_response_link {

  public:

    big_end_uint32  frag;
    big_end_uint32  xid;
    big_end_uint32  msg_type;
    big_end_uint32  reply_state;
    big_end_uint32  verifier_l;
    big_end_uint32  verifier_h;
    big_end_uint32  rpc_status;
    big_end_uint32  error_code;
    big_end_uint32  link_id;
    big_end_uint32  abort_port;
    big_end_uint32  max_receive_size;

};


int handle_lxi ( WiFiClient client )
{
  return 0;
}


int handle_udp ( WiFiUDP udp )
{
  int             len;
  udp_response *  response = (udp_response *)packet_buffer;
  udp_request *   request = (udp_request *)packet_buffer;

  std::stringstream  s;

  len = udp.read(packet_buffer, PACKET_BUFFER_SIZE);

  if ( len > 0 ) {
    telnet.println(dump(packet_buffer,len).c_str());

    if ( request->procedure != id_get_port )
      return -1;
    if ( request->getport_program != id_vxi_core )
      return -1;

    response->msg_type = 1;
    response->reply_state = 0;
    response->verifier_l = 0;
    response->verifier_h = 0;
    response->rpc_status = 0;
    response->vxi_port = uint32_t(port);

    len = sizeof(udp_response);

    s = std::stringstream();
    s << len;
    telnet.println(s.str().c_str());


    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(packet_buffer,len);
    udp.endPacket();

    telnet.println(dump(packet_buffer,len).c_str());
  }

  return 0;
}