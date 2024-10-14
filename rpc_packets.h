#ifndef RPC_PACKETS_H
#define RPC_PACKETS_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "utilities.h"

/*** basic functions to get and send packets ****************

    The get functions take the connection (UDP or TCP client),
    read the available data, and return the length of data
    received and stored in the data_buffer.

    The send functions take the connection (UDP or TCP client)
    and the length of the data to send; they send the data
    and return void.
  
****************************************************************/

uint32_t get_bind_packet ( WiFiUDP & udp );
uint32_t get_bind_packet ( WiFiClient & tcp );
uint32_t get_vxi_packet ( WiFiClient & tcp );

void send_bind_packet ( WiFiUDP & udp, uint32_t len );
void send_bind_packet ( WiFiClient & tcp, uint32_t len );
void send_vxi_packet ( WiFiClient & tcp, uint32_t len );

void fill_response_header ( uint8_t * buffer, uint32_t xid );

/*  primary data buffers used to send and receive packets  */

enum packet_buffer_sizes
{
  UDP_READ_SIZE = 64,     // The UDP bind request should be 56 bytes
  UDP_SEND_SIZE = 32,     // The UDP bind response should be 28 bytes 
  TCP_READ_SIZE = 64,     // The TCP bind request should be 56 bytes + 4 bytes for prefix
  TCP_SEND_SIZE = 32,     // The TCP bind response should be 28 bytes + 4 bytes for prefix
  VXI_READ_SIZE = 256,    // The VXI requests should never exceed 128 bytes, but extra allowed
  VXI_SEND_SIZE = 256     // The VXI responses should never exceed 128 bytes, but extra allowed
};

extern uint8_t  udp_read_buffer[];      // only for udp bind requests
extern uint8_t  udp_send_buffer[];      // only for udp bind responses
extern uint8_t  tcp_read_buffer[];      // only for tcp bind requests
extern uint8_t  tcp_send_buffer[];      // only for tcp bind responses
extern uint8_t  vxi_read_buffer[];      // only for vxi requests
extern uint8_t  vxi_send_buffer[];      // only for vxi responses

/*  constant variables to allow access to the portions of the data_buffer
    that represent prefix or packet for UDP and TCP communication  */

uint8_t * const udp_request_packet_buffer   = udp_read_buffer;
uint8_t * const udp_response_packet_buffer  = udp_send_buffer;

uint8_t * const tcp_request_prefix_buffer   = tcp_read_buffer;      // tcp prefix (length + frag) goes in first four bytes
uint8_t * const tcp_request_packet_buffer   = tcp_read_buffer + 4;  // packet data goes in remainder of buffer

uint8_t * const tcp_response_prefix_buffer  = tcp_send_buffer;      // tcp prefix (length + frag) goes in first four bytes
uint8_t * const tcp_response_packet_buffer  = tcp_send_buffer + 4;  // packet data goes in remainder of buffer

uint8_t * const vxi_request_prefix_buffer   = vxi_read_buffer;      // vxi prefix (length + frag) goes in first four bytes
uint8_t * const vxi_request_packet_buffer   = vxi_read_buffer + 4;  // packet data goes in remainder of buffer

uint8_t * const vxi_response_prefix_buffer  = vxi_send_buffer;      // vxi prefix (length + frag) goes in first four bytes
uint8_t * const vxi_response_packet_buffer  = vxi_send_buffer + 4;  // packet data goes in remainder of buffer

/*  Helpful information on the structure of the basic RPC packet is available
    at https://www.ibm.com/docs/it/aix/7.2?topic=concepts-remote-procedure-call.
    For information on the VXI specific packets, see the VXIbus TCP/IP Instrument
    Protocol Specification at https://vxibus.org/specifications.html.  */


/*  structures to allow description of / access to the data_buffer according to the type of packet  */

struct tcp_prefix_packet
{
  big_end_uint32  length;           // for tcp packets, this prefix contains a FRAG bit (0x80000000) and the length of the following packet
};

struct rpc_request_packet
{
  big_end_uint32  xid;              // transaction id (should be checked to make sure it matches, but we will just pass it back)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  rpc_version;      // rpc program version (should be 2, but we can ignore)
  big_end_uint32  program;          // program code - what program is being called
  big_end_uint32  program_version;  // program version (we can ignore)
  big_end_uint32  procedure;        // procedure code - what procedure is requested
  big_end_uint32  credentials_l;    // security codes (not used in this context)    
  big_end_uint32  credentials_h;    // security codes (not used in this context)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
};

struct rpc_response_packet
{
  big_end_uint32  xid;              // transaction id (we just pass it back what we received)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  reply_state;      // accepted or denied - we can indicate an error for accepted messages via rpc_status)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  rpc_status;       // status of accepted message: SUCCESS, PROG_UNAVAIL, etc.
};

struct bind_request_packet
{
  big_end_uint32  xid;              // transaction id (should be checked to make sure it matches, but we will just pass it back)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  rpc_version;      // rpc program version (should be 2, but we can ignore)
  big_end_uint32  program;          // program code - what program is being called
  big_end_uint32  program_version;  // program version (we can ignore)
  big_end_uint32  procedure;        // procedure code - what procedure is requested
  big_end_uint32  credentials_l;    // security codes (not used in this context)    
  big_end_uint32  credentials_h;    // security codes (not used in this context)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  getport_program;  // we can ignore this
  big_end_uint32  getport_version;  // we can ignore this
  big_end_uint32  getport_protocol; // we can ignore this
  big_end_uint32  getport_port;     // we can ignore this
};

struct bind_response_packet
{
  big_end_uint32  xid;              // transaction id (we just pass it back what we received)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  reply_state;      // accepted or denied - we can indicate an error for accepted messages via rpc_status)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  rpc_status;       // status of accepted message: SUCCESS, PROG_UNAVAIL, etc.
  big_end_uint32  vxi_port;         // the port that has been assigned for the remainder of the transaction
};

struct link_request_packet
{
  big_end_uint32  xid;              // transaction id (should be checked to make sure it matches, but we will just pass it back)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  rpc_version;      // rpc program version (should be 2, but we can ignore)
  big_end_uint32  program;          // program code - what program is being called
  big_end_uint32  program_version;  // program version (we can ignore)
  big_end_uint32  procedure;        // procedure code - what procedure is requested
  big_end_uint32  credentials_l;    // security codes (not used in this context)    
  big_end_uint32  credentials_h;    // security codes (not used in this context)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  client_id;        // we can ignore this (implementation specific id)
  big_end_uint32  lockDevice;       // request to lock device; we will ignore this
  big_end_uint32  lock_timeout;     // time to wait for the lock; we will ignore this
  big_end_uint32  data_len;         // length of the string in data
  char            data[];           // name of the instrument
};

struct link_response_packet
{
  big_end_uint32  xid;              // transaction id (we just pass it back what we received)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  reply_state;      // accepted or denied - we can indicate an error for accepted messages via rpc_status)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  rpc_status;       // status of accepted message: SUCCESS, PROG_UNAVAIL, etc.
  big_end_uint32  error;            // error code such as lock request timeout, out of resources, etc.
  big_end_uint32  link_id;          // a unique link id to be used by subsequent calls in this transaction; we can use 0
  big_end_uint32  abort_port;       // port to communicate with to abort procedures later in this transaction
  big_end_uint32  max_receive_size; // maximum amount of data that can be received on a write command
};

struct read_request_packet
{
  big_end_uint32  xid;              // transaction id (should be checked to make sure it matches, but we will just pass it back)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  rpc_version;      // rpc program version (should be 2, but we can ignore)
  big_end_uint32  program;          // program code - what program is being called
  big_end_uint32  program_version;  // program version (we can ignore)
  big_end_uint32  procedure;        // procedure code - what procedure is requested
  big_end_uint32  credentials_l;    // security codes (not used in this context)    
  big_end_uint32  credentials_h;    // security codes (not used in this context)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  link_id;          // unique link id supplied by the link response; we will use 0
  big_end_uint32  request_size;     // maximum (?) amount of data requested (we will ignore)
  big_end_uint32  io_timeout;       // how long to wait before giving up (we will ignore)
  big_end_uint32  lock_timeout;     // how long to wait to be able to lock the device (we will ignore)
  big_end_uint32  flags;            // can indicate whether a termination character is supplied (we will ignore)
  char            term_char;        // terminating character (we will ignore)
};

struct read_response_packet
{
  big_end_uint32  xid;              // transaction id (we just pass it back what we received)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  reply_state;      // accepted or denied - we can indicate an error for accepted messages via rpc_status)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  rpc_status;       // status of accepted message: SUCCESS, PROG_UNAVAIL, etc.
  big_end_uint32  error;            // error code such as lock request timeout, invalid link id, etc.
  big_end_uint32  reason;           // what terminated the data read - END of data, terminating character (CHR), or reached max size (REQCNT)
  big_end_uint32  data_len;         // length of the data returned
  char            data[];           // data returned
};

struct write_request_packet
{
  big_end_uint32  xid;              // transaction id (should be checked to make sure it matches, but we will just pass it back)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  rpc_version;      // rpc program version (should be 2, but we can ignore)
  big_end_uint32  program;          // program code - what program is being called
  big_end_uint32  program_version;  // program version (we can ignore)
  big_end_uint32  procedure;        // procedure code - what procedure is requested
  big_end_uint32  credentials_l;    // security codes (not used in this context)    
  big_end_uint32  credentials_h;    // security codes (not used in this context)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  link_id;          // unique link id supplied by the link response; we will use 0
  big_end_uint32  io_timeout;       // how long to wait before giving up (we will ignore)
  big_end_uint32  lock_timeout;     // how long to wait to be able to lock the device (we will ignore)
  big_end_uint32  flags;            // can indicate whether an END character is used (we will ignore)
  big_end_uint32  data_len;         // length of data sent
  char            data[];           // data sent
};

struct write_response_packet
{
  big_end_uint32  xid;              // transaction id (we just pass it back what we received)
  big_end_uint32  msg_type;         // message type - CALL or REPLY
  big_end_uint32  reply_state;      // accepted or denied - we can indicate an error for accepted messages via rpc_status)
  big_end_uint32  verifier_l;       // security codes (not used in this context)
  big_end_uint32  verifier_h;       // security codes (not used in this context)
  big_end_uint32  rpc_status;       // status of accepted message: SUCCESS, PROG_UNAVAIL, etc.
  big_end_uint32  error;            // error code such as lock request timeout, invalid link id, etc.
  big_end_uint32  size;             // number of bytes written
};

/*  constant variables used to access the data_buffer as the various structures defined above  */

rpc_request_packet    * const udp_request         = (rpc_request_packet *)    udp_request_packet_buffer;
rpc_response_packet   * const udp_response        = (rpc_response_packet *)   udp_response_packet_buffer;

bind_request_packet   * const udp_bind_request    = (bind_request_packet *)   udp_request_packet_buffer;
bind_response_packet  * const udp_bind_response   = (bind_response_packet *)  udp_response_packet_buffer;

rpc_request_packet    * const tcp_request         = (rpc_request_packet *)    tcp_request_packet_buffer;
rpc_response_packet   * const tcp_response        = (rpc_response_packet *)   tcp_response_packet_buffer;

tcp_prefix_packet     * const tcp_request_prefix  = (tcp_prefix_packet *)     tcp_request_prefix_buffer;
tcp_prefix_packet     * const tcp_response_prefix = (tcp_prefix_packet *)     tcp_response_prefix_buffer;

bind_request_packet   * const tcp_bind_request    = (bind_request_packet *)   tcp_request_packet_buffer;
bind_response_packet  * const tcp_bind_response   = (bind_response_packet *)  tcp_response_packet_buffer;

rpc_request_packet    * const vxi_request         = (rpc_request_packet *)    vxi_request_packet_buffer;
rpc_response_packet   * const vxi_response        = (rpc_response_packet *)   vxi_response_packet_buffer;

tcp_prefix_packet     * const vxi_request_prefix  = (tcp_prefix_packet *)     vxi_request_prefix_buffer;
tcp_prefix_packet     * const vxi_response_prefix = (tcp_prefix_packet *)     vxi_response_prefix_buffer;

link_request_packet   * const link_request        = (link_request_packet *)   vxi_request_packet_buffer;
link_response_packet  * const link_response       = (link_response_packet *)  vxi_response_packet_buffer;

read_request_packet   * const read_request        = (read_request_packet *)   vxi_request_packet_buffer;
read_response_packet  * const read_response       = (read_response_packet *)  vxi_response_packet_buffer;

write_request_packet  * const write_request       = (write_request_packet *)  vxi_request_packet_buffer;
write_response_packet * const write_response      = (write_response_packet *) vxi_response_packet_buffer;

#endif