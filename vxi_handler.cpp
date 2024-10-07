#include "vxi_server.h"
#include "vxi_handler.h"
#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"


void VXI_Handler::begin ( uint32_t port )
{
  tcp_server.stop();
  tcp_server.begin(port);

  vxi_port = port;
  last_packet_time = millis();

  Debug.Progress() << "Listening for VXI commands on TCP port " << port << "\n\n";
}


void VXI_Handler::process ()
{
  if ( vxi_port )
  {
    bool  bClose = false;

    if ( client )
    {
      int len = get_packet(client);

      if ( len > 0 )
      {
        last_packet_time = millis();

        bClose = handle_packet(client);
      }
    }
    else 
    {
      client = tcp_server.accept();
      last_packet_time = millis();

      if ( client ) {
        Debug.Progress() << "VXI connection established on port " << vxi_port << "\n\n";
      }
    }

    if ( ( millis() - last_packet_time ) > VXI_Server::get_timeout() )
    {
      Debug.Progress() << "Timeout: ";
      bClose = true;
    }

    if ( bClose )
    {
      Debug.Progress() << "Closing VXI connection on port " << vxi_port << "\n\n";

      client.stop();
      tcp_server.stop();
      vxi_port = 0;
    }
  } // endif vxi_port
}


bool VXI_Handler::handle_packet ( WiFiClient & client )
{
  bool      bClose = false;
  uint32_t  rc = rpc::SUCCESS;

  if ( rpc_request->program != rpc::VXI_11_CORE )
  {
    rc = rpc::PROG_UNAVAIL;

    Debug.Error() << "Invalid program (expected VXI_11_CORE = 0x607AF; received ";
    Debug.printf("0x%08x)\n\n",(uint32_t)(rpc_request->program));

  }
  else switch ( rpc_request->procedure )
  {
    case rpc::VXI_11_CREATE_LINK:

      create_link(client);
      break;
          
    case rpc::VXI_11_DEV_READ:

      read(client);
      break;

    case rpc::VXI_11_DEV_WRITE:

      write(client);
      break;

    case rpc::VXI_11_DESTROY_LINK:

      destroy_link(client);
      bClose = true;
      break;

    default:

      Debug.Error() << "Invalid VXI-11 procedure (received " << (uint32_t)(rpc_request->procedure) << ")\n\n";

      rc = rpc::PROC_UNAVAIL;
      break;
  }

  if ( rc != rpc::SUCCESS )
  {
    rpc_response->rpc_status = rc;
    send_packet(client, sizeof(rpc_response_packet));
  }

  /*  signal to caller whether the port should be closed  */

  return bClose;
}


void VXI_Handler::parse_scpi ( char * scpi ) {

}


void VXI_Handler::create_link ( WiFiClient & client )
{
  /*  The data field in a link request should contain a string
      with the name of the requesting device. It may already
      be null-terminated, but just in case, we will put in
      the terminator.  */
  
  link_request->data[link_request->data_len] = 0;

  Debug.Progress() << "CREATE LINK request from \"" << link_request->data << "\" on port " << vxi_port << "\n";

  /*  Generate the response  */

  link_response->rpc_status = rpc::SUCCESS;
  link_response->error = rpc::NO_ERROR;
  link_response->link_id = 0;
  link_response->abort_port = 0;
  link_response->max_receive_size = PACKET_BUFFER_SIZE;

  send_packet(client, sizeof(link_response_packet));
}


void VXI_Handler::destroy_link ( WiFiClient & client )
{
  Debug.Progress() << "DESTROY LINK on port " << vxi_port << "\n";
}


void VXI_Handler::read ( WiFiClient & client )
{
  /*  We could and should respond to a BSWV? command with the current
      AWG settings. However, these seem to be ignored. The only
      response that actually matters is the AWG identification string,
      so we will

      Generate the response  */

  char      AWG_ID[] = "IDN-SGLT-PRI SDG1062X";   // ID used to simulate a Siglent AWG
  uint32_t  len = strlen(AWG_ID);

  Debug.Progress() << "READ DATA on port " << vxi_port << "; data sent = " << AWG_ID << "\n";  

  read_response->rpc_status = rpc::SUCCESS;
  read_response->error = rpc::NO_ERROR;
  read_response->reason = rpc::END;
  read_response->data_len = len;
  strcpy(read_response->data,AWG_ID);

  send_packet(client, sizeof(read_response_packet) + len);
}


void VXI_Handler::write ( WiFiClient & client )
{
  uint32_t  len = write_request->data_len;

  /*  The data field in a write request should contain a string
      with the command for the AWG. It may already be null-
      terminated, but just in case, we will put in the terminator.  */

  write_request->data[len] = 0;

  Debug.Progress() << "WRITE DATA on port " << vxi_port << " = " << write_request->data << "\n";

  /*  Parse and respond to the SCPI command  */

  parse_scpi(write_request->data);

  /*  Generate the response  */

  write_response->rpc_status = rpc::SUCCESS;
  write_response->error = rpc::NO_ERROR;
  write_response->size = len;

  send_packet(client, sizeof(write_response_packet));
}
