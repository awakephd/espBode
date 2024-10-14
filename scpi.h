#ifndef SCPI_H
#define SCPI_H


namespace scpi {

/*** SCPI Initiators ***************************************

  These initiate the SCPI command line. We will process
  either a channel-based command sequence initiated by C1
  or C2, or an identification request.

***********************************************************/

const char * const initiators[] = { "IDN-SGLT-PRI?",
                                    "C"
                                  };

enum initiator_id {
  ID_REQUEST        = 0,
  CHANNEL           = 1,
  initiator_id_cnt  = 2
};

/*** SCPI channel commands *********************************

  The following commands will follow a channel designation
  in the form of C1: or C2:

************************************************************/

const char * const commands[] = { "OUTP",
                                  "BSWV?",
                                  "BSWV"
                                };

enum command_id {
  SET_OUTPUT      = 0,
  GET_PARAMETERS  = 1,
  SET_PARAMETERS  = 2,
  command_id_cnt  = 3
};

/*** SCPI BSWV parameters **********************************

  The following commands will follow the BSWV command. There
  are other possible commands, but these are the only ones
  we will process.

************************************************************/

const char * const parameters[] = { "OFF",
                                    "ON",
                                    "WVTP",
                                    "FRQ",
                                    "AMP",
                                    "OFST",
                                    "PHSE"
                                  };

enum parameter_id {
  OUTPUT_OFF        = 0,
  OUTPUT_ON         = 1,
  WAVE              = 2,
  FREQUENCY         = 3,
  AMPLITUDE         = 4,
  OFFSET            = 5,
  PHASE             = 6,
  parameter_count   = 7
};

/*** SCPI delimiters ****************************************

  The following characters are used to separate parts of the
  SCPI text: the initiator (":"), the command (";""), the
  command from the parameters (" "), and the parameters from
  each other (",")

************************************************************/

const char * const delimiters[] = { ":",
                                    ";",
                                    " ",
                                    ","
                                  };

enum delimiter_id {
  INITIATOR         = 0,
  COMMAND           = 1,
  PRE_PARAMETERS    = 2,
  PARAMETERS        = 3,
  delimiter_id_cnt  = 4
};

}; // end namespace

#endif
