#include "vxi_server.h"
#include "rpc_packets.h"
#include "rpc_enums.h"
#include "debug.h"
#include "scpi.h"


VXI_Server::VXI_Server ()
  : vxi_port(rpc::VXI_PORT_START, rpc::VXI_PORT_END)
{
  /*  We do not start the tcp_server port here, because
      WiFi has likely not yet been initialized. Instead,
      we wait until the begin() command.  */
}


VXI_Server::~VXI_Server ()
{
}


uint32_t VXI_Server::allocate ()
{
  uint32_t  port = 0;

  if ( available() ) {
    port = vxi_port;

//    Debug.Progress() << "Listening for VXI commands on TCP port " << port << "\n";
  }

  return port;
}


void VXI_Server::begin ( bool bNext )
{
  if ( bNext )
  {
    client.stop();
    tcp_server.stop();

    /*  Note that vxi_port is not an ordinary uint32_t. It is
        an instance of class cyclic_uint32, defined in utilities.h,
        which is constrained to a range of values. The increment
        operator will cause it to go to the next value, automatically
        going back to the starting value once it exceeds the maximum
        of its range.  */

    vxi_port++;
  }
  
  tcp_server.begin(vxi_port);

  Debug.Progress() << "\nListening for VXI commands on TCP port " << vxi_port << "\n";
}


void VXI_Server::loop ()
{
  if ( client )      // if a connection has been established on port
  {
    bool  bClose = false;
    int   len = get_packet(client);

    if ( len > 0 ) {
      bClose = handle_packet();
    }
    
    if ( bClose ) {

      Debug.Progress() << "Closing VXI connection on port " << vxi_port << "\n";

      /*  this method will stop the client and the tcp_server, then rotate
          to the next port (within the specified range) and restart the
          tcp_server to listen on that port.  */

      begin_next();
    }
  }
  else  // i.e., if ! client
  {
    client = tcp_server.accept();   // see if a client is available (data has been sent on port)

    if ( client ) {
      Debug.Progress() << "\nVXI connection established on port " << vxi_port << "\n";
    }
  }
}


bool VXI_Server::handle_packet ()
{
  bool      bClose = false;
  uint32_t  rc = rpc::SUCCESS;

  if ( rpc_request->program != rpc::VXI_11_CORE )
  {
    rc = rpc::PROG_UNAVAIL;

    Debug.Error() << "Invalid program (expected VXI_11_CORE = 0x607AF; received ";
    Debug.printf("0x%08x)\n",(uint32_t)(rpc_request->program));

  }
  else switch ( rpc_request->procedure )
  {
    case rpc::VXI_11_CREATE_LINK:

      create_link();
      break;
          
    case rpc::VXI_11_DEV_READ:

      read();
      break;

    case rpc::VXI_11_DEV_WRITE:

      write();
      break;

    case rpc::VXI_11_DESTROY_LINK:

      destroy_link();
      bClose = true;
      break;

    default:

      Debug.Error() << "Invalid VXI-11 procedure (received " << (uint32_t)(rpc_request->procedure) << ")\n";

      rc = rpc::PROC_UNAVAIL;
      break;
  }

  /*  Response messages will be sent by the various routines above
      when the program and procedure are recognized (and therefore
      rc == rpc::SUCCESS). We only need to send a response here
      if rc != rpc::SUCCESS.  */

  if ( rc != rpc::SUCCESS )
  {
    rpc_response->rpc_status = rc;
    send_packet(client, sizeof(rpc_response_packet));
  }

  /*  signal to caller whether the connection should be close (i.e., DESTROY_LINK)  */

  return bClose;
}


void VXI_Server::create_link ()
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


void VXI_Server::destroy_link ()
{
  Debug.Progress() << "DESTROY LINK on port " << vxi_port << "\n";
}


void VXI_Server::read ()
{
  /*  We could and should respond to a BSWV? command with the current
      AWG settings. The scpi_parse command does identify the type
      of read needed using the read_type member variable. However,
      the actual read response seems to be ignored except when the
      scope asks for the AWG identification string. Since that string
      can satisfy every read request, we will just generate it every
      time.  */

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


void VXI_Server::write ()
{
  uint32_t  len = write_request->data_len;

  /*  The data field in a write request should contain a string
      with the command for the AWG. It may end with '\n', which
      we will filter out to avoid inconsistent formatting in our
      Debug output. It may already be null-terminated, but just
      in case, or in case we have filtered out '\n', we will add
      in the terminator.  */

  while ( len > 0 && write_request->data[len-1] == '\n' ) {
    len--;
  }

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

/*** parse_scpi() ******************************************

  This method parses the SCPI commands and issues the
  appropriate commands to the AWG.

  The logic is as follows:

  First, tokenize the initiator. If the initiator is the
  identification request, set the flag for the next read
  request and return. If it is the channel initiator,
  get the channel and proceed to the next step.

  Second, tokenize the remainder of the buffer to look
  for a command. If it has parameters (OUTP and BSWV),
  call process_parameters to get the parameters (ON, OFF,
  or <parameter name>,<value> pairs), and set the AWG
  accordingly. If the command is BSWV?, set the flag
  for the next read request.

  Repeat step 2 until there are no more commands left
  to process.

  Note that this function uses the re-entrant strtok_r,
  not to be thread-safe (we do not anticipate multi-
  threading on the ESP-01), but to allow an inner and
  outer loop to work simultaneously.

***********************************************************/

void VXI_Server::parse_scpi ( char * buffer )
{
  char *  initiator;
  char *  command_line;
  char *  command;
  char *  command_context;
  char *  parameter_context;
  int     id;

  rw_channel = 0;
  read_type = rt_none;

  // first, get the initiator and its id

  initiator = strtok_r(buffer, scpi::delimiters[scpi::INITIATOR], &command_context);
  id = get_id(initiator, scpi::initiators, scpi::initiator_id_cnt);

  // process according to the id of the initiator
  switch ( id ) {

    /*  if initializer = IDN-SGLT-PRI? then set read_type flag and
        return - no further processing needed  */

    case scpi::ID_REQUEST:

      read_type = rt_identification;
      return;

    /*  if initializer = C, extract the channel and drop down
        to do further processing  */

    case scpi::CHANNEL:

      sscanf(initiator+1, "%d", &rw_channel);
      break;

    // if neither, we don't recognize this initiator, so we simply return

    default:

      return;

  } // end switch ( initiator id )

  /*  If we get to this point, we have received the channel and are ready
      to process command lines. The format of each command_line will be
      COMMAND<space>PARAMETER,VALUE[,PARAMETER,VALUE ...]  */

  command_line = strtok_r(NULL, scpi::delimiters[scpi::COMMAND], &command_context);

  while ( command_line != NULL ) {

    // extract the actual command from the command_line and get its id

    command = strtok_r(command_line, scpi::delimiters[scpi::PRE_PARAMETERS], &parameter_context);
    id = get_id(command, scpi::commands, scpi::command_id_cnt);

    switch ( id ) {

      /*  if id = OUTP, process parameters looking for "ON" or "OFF"
          and set AWG accordingly  */

      case scpi::SET_OUTPUT:

        process_parameters(parameter_context);
        break;

      // if id = BSWV, process wave parameters and set AWG accordingly

      case scpi::SET_PARAMETERS:

        process_parameters(parameter_context);
        break;

      // if id = BSWV?, set flag so that next read retrieves wave parameters

      case scpi::GET_PARAMETERS:

        read_type = rt_parameters;
        break;

      // if none of the above, we don't recognize the command, so we ignore it

      default:

        break;

    } // end switch ( command id )

    // extract the next command_line; if any, cycle back through the loop

    command_line = strtok_r(NULL, scpi::delimiters[scpi::COMMAND], &command_context);

  } // end while ( command_line != NULL )
}

/*** process_parameters()********************************

  This method continues to tokenize the command_line via
  the supplied parameter_context. It expects to see
  either ON, OFF, or a parameter pair (name,value). If
  it recognizes the parameter, it sets the AWG accordingly.
  It continues until there are no more parameters to
  process.

********************************************************/

void VXI_Server::process_parameters ( char * parameter_context )
{
  char *  parameter;
  char *  s_val;
  double  value;
  int     id;

  /*  Note that strtok_r here usese NULL for the initial argument,
      because it is continuing to tokenize the line begun in
      parse_scpi and pointed to by the parameter_context. This
      also means that we can get the parameter in the while test. */

  Debug.Progress() << "Setting AWG Channel " << rw_channel << ": ";

  while ( ( parameter = strtok_r ( NULL, scpi::delimiters[scpi::PARAMETERS], &parameter_context ) ) != NULL ) {

    // translate the parameter into a parameter id or -1 if not one we know

    id = get_id ( parameter, scpi::parameters, scpi::parameter_id_cnt );

    // if the parameter is one we recognize, process it

    if ( id >= 0 ) {

      // if the parameter is not ON or OFF, we need to get the following value

      if ( id > scpi::OUTPUT_ON ) {
        s_val = strtok_r ( NULL, scpi::delimiters[scpi::PARAMETERS], &parameter_context );
        sscanf(s_val, "%lf", &value);

        Debug.Progress() << parameter << " = " << value << "; ";
      }
      
      // if it is ON or OFF, let value = 0 (OFF) or 1 (ON)

      else {
        value = id;

        Debug.Progress() << "OUTPUT = " << ( id == scpi::OUTPUT_ON ? "ON" : "OFF" ) << "; ";
      }

      // ask the AWG to set the parameter and verify the result

    } // end if valid id

  } // end while parameter != NULL

  Debug.Progress() << "\n";
}

/*** get_id() ******************************************

  This method matches the id against one of the ids in
  the supplied list. IF there is a match, it returns its
  index; otherwise, it returns -1.

********************************************************/

int VXI_Server::get_id ( const char * id_text, const char * id_list[], size_t id_cnt )
{
  int id = -1;

  for ( int i = 0; i < id_cnt; i++ ) {
    if ( strncmp(id_text, id_list[i], strlen(id_list[i])) == 0 ) {
      id = i;
      break;
    }
  }

  return id;
}
