#ifndef SCPI_H
#define SCPI_H

/*!
  @file   scpi.h
  @brief  Declaration of tables and enumerators used
          to decode SCPI commands.
*/

/*!
  @brief  The scpi namespace distinguishes identifiers used for SCPI decoding.
*/
namespace scpi {

/*!
  @brief  Character strings that initiate the SCPI command line.

  We are concerned with only two types of SCPI commands. The
  first is a request for identification of the AWG (i.e., to
  appear as a valid Siglent AWG), and the other is a channel-
  based command sequence, in which the C will be followed by
  the channel number, a colon, and one or more "channel
  commands" to turn on or off the output, set the wave
  parameters, etc.
*/
const char * const initiators[] = { "IDN-SGLT-PRI?",
                                    "C"
                                  };

/*!
  @brief  Enumeration to provide id's for the entries in the scpi::initiators array.
*/
enum initiator_id {
  ID_REQUEST        = 0,    ///< The SCPI line is a request for the AWG identifier
  CHANNEL           = 1,    ///< The SCPI line is a channel-based set of commands
  initiator_id_cnt  = 2     ///< The number of initiator id's
};

/*!
  @brief  Commands that can follow a channel initiator
          (e.g., C1: or C2:)

  Note that there are other commands than those listed below
  (ARWV, SYNC, and so on) which we will not process.
*/
const char * const commands[] = { "OUTP",     // output on or off
                                  "BSWV?",    // request for current wave parameter settings
                                  "BSWV"      // set wave parameters
                                };

/*!
  @brief  Enumeration to provide id's for the entries in scpi::commands array.
*/
enum command_id {
  SET_OUTPUT      = 0,    ///< Turn channel output on or off
  GET_PARAMETERS  = 1,    ///< Return the current channel wave parameters
  SET_PARAMETERS  = 2,    ///< Set wave parameters on the current channel
  command_id_cnt  = 3     ///< The number of command id's
};

/*!
  @brief  Parameters that can follow the "channel commands."

  The first two parameters may follow the OUTP command, while
  the rest may follow the BSWV command. Note that there are
  other possible parameters, but these are the only ones we will
  process.
*/
const char * const parameters[] = { "OFF",    // output off
                                    "ON",     // output on
                                    "WVTP",   // set wave type
                                    "FRQ",    // set frequency
                                    "AMP",    // set amplitude
                                    "OFST",   // set offset
                                    "PHSE"    // set phase
                                  };

/*!
  @brief  Enumeration to provide id's for the entries in scpi::parameters array.
*/
enum parameter_id {
  OUTPUT_OFF        = 0,    ///< Value following OUTP will be OFF
  OUTPUT_ON         = 1,    ///< Value following OUTP will be ON
  WAVE              = 2,    ///< Value following WVTP will be SINE (only SINE for Bode plot)
  FREQUENCY         = 3,    ///< Value following FRQ will be Hz as floating point number, e.g., 16387.24
  AMPLITUDE         = 4,    ///< Value following AMP will be voltage as floating point number, e.g., 3.5
  OFFSET            = 5,    ///< Value following OFST will be voltage as floating point number, e.g., -1.375
  PHASE             = 6,    ///< Value following PHSE will be degrees as floating point number, e.g., 27.5
  parameter_count   = 7     ///< The number of parameter id's
};

/*!
  @brief  Characters used to separate parts of the SCPI command line.
  
  The initiator is separated from the rest of the line by a colon
  ("C1:"); one "channel command" is separated from another by a semi-
  colon, and the list of parameters is separated from the command by
  a space ("OUTP ON; BSWV FRQ..."); parameters and values are separated
  from each other by a comma (FRQ,16387.24,AMP,3.5...)
*/
const char * const delimiters[] = { ":",
                                    ";",
                                    " ",
                                    ","
                                  };

/*!
  @brief  Enumeration to provide id's for the entries in scpi::parameters array.
*/
enum delimiter_id {
  INITIATOR         = 0,    ///< A colon separates the initiator from the remainder of the SCPI line
  COMMAND           = 1,    ///< A semicolon separates each channel command from the next
  PRE_PARAMETERS    = 2,    ///< A space follows the channel command before the parameter
  PARAMETERS        = 3,    ///< A comma separates each parameter and value
  delimiter_id_cnt  = 4     ///< The number of delimiter id's
};

}; // end namespace

#endif
