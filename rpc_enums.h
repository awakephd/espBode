#ifndef RPC_ENUMS_H
#define RPC_ENUMS_H

/*  This file contains RPC and VXI protocol enumerations

    Helpful information on the protocol of the basic RPC packet is available
    at https://www.ibm.com/docs/it/aix/7.2?topic=concepts-remote-procedure-call.
    For information on the VXI specific protocol, see the VXIbus TCP/IP Instrument
    Protocol Specification at https://vxibus.org/specifications.html.  */


namespace rpc {

/*  ports  */

enum ports {

  BIND_PORT       = 111,
  VXI_PORT_START  = 9010,
  VXI_PORT_END    = 9019
};

/*  message types  */

enum msg_type {

  CALL  = 0,
  REPLY = 1
};

/*  values to return in reply_state  */

enum reply_state {

  MSG_ACCEPTED  = 0,
  MSG_DENIED    = 1
};

/*  values to use for denied messages  */

enum reject_status {

  RPC_MISMATCH  = 0,
  AUTH_ERROR    = 1
};

enum auth_status {

  AUTH_BADCRED      = 1,
  AUTH_REJECTEDCRED = 2,
  AUTH_BADVERF      = 3,
  AUTH_REJECTEDVERF = 4,
  AUTH_TOOWEAK      = 5
};

/*  values to use for accepted messages  */

enum rpc_status {

  SUCCESS       = 0,
  PROG_UNAVAIL  = 1,
  PROG_MISMATCH = 2,
  PROC_UNAVAIL  = 3,
  GARBAGE_ARGS  = 4
};

/*  program codes  */

enum programs {

  PORTMAP     = 0x186A0,
  VXI_11_CORE = 0x607AF
};

/*  procedure codes  */

enum procedures {

  GET_PORT            = 3,
  VXI_11_CREATE_LINK  = 10,
  VXI_11_DEV_WRITE    = 11,
  VXI_11_DEV_READ     = 12,
  VXI_11_DESTROY_LINK = 23
};

/*  error codes  */

enum errors {

  NO_ERROR          = 0,
  SYNTAX_ERROR      = 1,
  NOT_ACCESSIBLE    = 3,
  INVALID_LINK      = 4,
  PARAMETER_ERROR   = 5,
  NO_CHANNEL        = 6,
  INVALID_OPERATION = 8,
  OUT_OF_RESOURCES  = 9,
  DEVICE_LOCKED     = 11,
  NO_LOCK_HELD      = 12,
  IO_TIMEOUT        = 15,
  LOCK_TIMEOUT      = 17,
  INVALID_ADDRESS   = 21,
  ABORT             = 23,
  DUPLICATE_CHANNEL = 29
};

/*  reason codes for read respone  */

enum reasons {

  END     = 4,      // read reached end of data
  CHR     = 2,      // read reached terminating character supplied in the read request packet
  REQCNT  = 1       // read reached the maximum count requested
};

}; // end namespace

#endif