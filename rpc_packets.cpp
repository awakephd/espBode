#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"


/*  buffers to hold packet data  */

uint8_t  udp_read_buffer[UDP_READ_SIZE];      // only for udp bind requests
uint8_t  udp_send_buffer[UDP_SEND_SIZE];      // only for udp bind responses
uint8_t  tcp_read_buffer[TCP_READ_SIZE];      // only for tcp bind requests
uint8_t  tcp_send_buffer[TCP_SEND_SIZE];      // only for tcp bind responses
uint8_t  vxi_read_buffer[VXI_READ_SIZE];      // only for vxi requests
uint8_t  vxi_send_buffer[VXI_SEND_SIZE];      // only for vxi responses

/*  fill response header with default response data  */

void fill_response_header ( uint8_t * buffer, uint32_t xid )
{
  rpc_response_packet * rpc_response = (rpc_response_packet *) buffer;

  rpc_response->xid = xid;                        // transaction id supplied by the request
  rpc_response->msg_type = rpc::REPLY;            // CALL = 0; REPLY = 1
  rpc_response->reply_state = rpc::MSG_ACCEPTED;  // MSG_ACCEPTED = 0; MSG_DENIED = 1
  rpc_response->verifier_l = 0;
  rpc_response->verifier_h = 0;
  //rpc_response->rpc_status = rpc::SUCCESS;      // status code will be supplied by the calling function
}

/*  basic functions to read and to send packet data  */

uint32_t get_bind_packet ( WiFiUDP & udp )
{
  uint32_t  len = udp.read(udp_request_packet_buffer, UDP_READ_SIZE);

  if ( len > 0 ) {
    Debug.Packet() << "\nReceived " << len << " bytes from " << udp.remoteIP().toString() << ":" << udp.remotePort() << "\n";
    Debug.Packet() << Debug.Dump(udp_request_packet_buffer,len) << "\n";
  }

  return len;
}

uint32_t get_bind_packet ( WiFiClient & client )
{
  uint32_t  len;

  tcp_request_prefix->length = 0;                     // set the length to zero in case the following read fails

  client.readBytes(tcp_request_prefix_buffer, 4);     // get the FRAG + LENGTH field

  len = ( tcp_request_prefix->length & 0x7fffffff );  // mask out the FRAG bit

  if ( len > 4 ) {
    len = std::min(len,(uint32_t)(TCP_READ_SIZE-4));              // do not read more than the buffer can hold

    client.readBytes(tcp_request_packet_buffer,len);

    Debug.Packet() << "\nReceived " << len+4 << " bytes from " << client.remoteIP().toString() << ":" << client.remotePort() << "\n";
    Debug.Packet() << Debug.Dump(tcp_request_prefix_buffer,len+4) << "\n";
  }

  return len;
}

uint32_t get_vxi_packet ( WiFiClient & client )
{
  uint32_t  len;

  vxi_request_prefix->length = 0;                     // set the length to zero in case the following read fails

  client.readBytes(vxi_request_prefix_buffer, 4);     // get the FRAG + LENGTH field

  len = ( vxi_request_prefix->length & 0x7fffffff );  // mask out the FRAG bit

  if ( len > 4 ) {
    len = std::min(len,(uint32_t)(VXI_READ_SIZE-4));              // do not read more than the buffer can hold

    client.readBytes(vxi_request_packet_buffer, len);

    Debug.Packet() << "\nReceived " << len+4 << " bytes from " << client.remoteIP().toString() << ":" << client.remotePort() << "\n";
    Debug.Packet() << Debug.Dump(vxi_request_prefix_buffer,len+4) << "\n";
  }

  return len;
}

void send_bind_packet ( WiFiUDP & udp, uint32_t len )
{
  fill_response_header(udp_response_packet_buffer, udp_request->xid);  // get the xid from the request

  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write(udp_response_packet_buffer,len);
  udp.endPacket();

  Debug.Packet() << "\nSent " << len << " bytes to " << udp.remoteIP().toString() << ":" << udp.remotePort() << "\n";
  Debug.Packet() << Debug.Dump(udp_response_packet_buffer,len) << "\n";
}

void send_bind_packet ( WiFiClient & tcp, uint32_t len )
{
  fill_response_header(tcp_response_packet_buffer, tcp_request->xid);  // get the xid from the request

  // adjust length to multiple of 4, appending 0's to fill the dword

  while ( (len & 3) > 0 ) {
    tcp_response_packet_buffer[len++] = 0;
  }

  tcp_response_prefix->length = 0x80000000 | len;   // set the FRAG bit and the length;

  while ( tcp.availableForWrite() == 0 );           // wait for tcp to be available

  tcp.write(tcp_response_prefix_buffer,len+4);      // add 4 to the length to accound for the tcp_response_prefix

  Debug.Packet() << "\nSent " << len << " bytes to " << tcp.remoteIP().toString() << ":" << tcp.remotePort() << "\n";
  Debug.Packet() << Debug.Dump(tcp_response_prefix_buffer,len+4) << "\n";
}

void send_vxi_packet ( WiFiClient & tcp, uint32_t len )
{
  fill_response_header(vxi_response_packet_buffer, vxi_request->xid);

  // adjust length to multiple of 4, appending 0's to fill the dword

  while ( (len & 3) > 0 ) {
    vxi_response_packet_buffer[len++] = 0;
  }

  vxi_response_prefix->length = 0x80000000 | len;   // set the FRAG bit and the length;

  while ( tcp.availableForWrite() == 0 );           // wait for tcp to be available

  tcp.write(vxi_response_prefix_buffer,len+4);      // add 4 to the length to account for the vxi_response_prefix

  Debug.Packet() << "\nSent " << len << " bytes to " << tcp.remoteIP().toString() << ":" << tcp.remotePort() << "\n";
  Debug.Packet() << Debug.Dump(vxi_response_prefix_buffer,len+4) << "\n";
}
