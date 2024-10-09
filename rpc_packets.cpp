#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"


/*  buffer to hold packet data  */

uint8_t data_buffer[PACKET_BUFFER_SIZE + 4];      // allow 4 extra bytes for tcp_prefix


/*  fill response header with default response data  */

void fill_response_header ()
{
  // rpc_response->xid = xid;                     // since the reply is sent immediately, the xid field from the request is still valid
  rpc_response->msg_type = rpc::REPLY;            // CALL = 0; REPLY = 1
  rpc_response->reply_state = rpc::MSG_ACCEPTED;  // MSG_ACCEPTED = 0; MSG_DENIED = 1
  rpc_response->verifier_l = 0;
  rpc_response->verifier_h = 0;
  //rpc_response->rpc_status = rpc::SUCCESS;      // status code will be supplied by the calling function
}

/*  basic functions to read and to send packet data  */

uint32_t get_packet ( WiFiUDP & udp )
{
  uint32_t  len = udp.read(packet_buffer, PACKET_BUFFER_SIZE);

  if ( len > 0 ) {
    Debug.Detail() << "\nReceived " << len << " bytes from " << udp.remoteIP().toString() << ":" << udp.remotePort() << "\n";
    Debug.Detail() << Debug.Dump(packet_buffer,len) << "\n";
  }

  return len;
}

uint32_t get_packet ( WiFiClient & client )
{
  uint32_t  len;

  tcp_prefix->length = 0;                       // set the length to zero in case the following read fails

  client.readBytes(tcp_prefix_buffer, 4);       // get the FRAG + LENGTH field

  len = (tcp_prefix->length & 0x7fffffff );     // mask out the FRAG bit

  if ( len > 4 ) {
    client.readBytes(packet_buffer,len);

    Debug.Detail() << "\nReceived " << len+4 << " bytes from " << client.remoteIP().toString() << ":" << client.remotePort() << "\n";
    Debug.Detail() << Debug.Dump(data_buffer,len+4) << "\n";
  }

  return len;
}

void send_packet ( WiFiUDP & udp, uint32_t len )
{
  fill_response_header();

  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write(packet_buffer,len);
  udp.endPacket();

  Debug.Detail() << "\nSent " << len << " bytes to " << udp.remoteIP().toString() << ":" << udp.remotePort() << "\n";
  Debug.Detail() << Debug.Dump(packet_buffer,len) << "\n";
}

void send_packet ( WiFiClient & tcp, uint32_t len )
{
  fill_response_header();

  // adjust length to multiple of 4, appending 0's to fill the dword

  while ( (len & 3) > 0 ) {
    packet_buffer[len++] = 0;
  }

  tcp_prefix->length = 0x80000000 | len;    // set the FRAG bit and the length;

  while ( tcp.availableForWrite() == 0 );   // wait for tcp to be available

  tcp.write(data_buffer,len+4);             // add 4 for the tcp_prefix

  Debug.Detail() << "\nSent " << len << " bytes to " << tcp.remoteIP().toString() << ":" << tcp.remotePort() << "\n";
  Debug.Detail() << Debug.Dump(data_buffer,len+4) << "\n";
}
