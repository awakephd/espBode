#ifndef AWG_FY_H
#define AWG_FY_H

#include "awg_server.h"

const int  awg_response_length = 20;

enum  param_translator_types
{
  pt_BOOL   = 0,
  pt_INT    = 1,
  pt_DOUBLE = 2
};

struct param_translator
{
  uint8_t   set_type;       // set value as integer, double, or text
  int8_t    set_exponent;   // multiply value by 10^exponent before sending
  uint8_t   set_precision;  // if double, how many decimal places
  uint8_t   set_width;      // if width != 0, indicates need to zero-fill to achieve width
  uint8_t   get_type;       // value read from AWG is integer, double, or text
  int8_t    get_exponent;   // value read from AWG must be multiplied by 10^exponent
};

/*** class AWG_FY **************************************

  The FeelTech AWGs all use a similar, but not entirely
  identical, command structure for getting and setting
  parameters. The variations can be described by a table
  of "parameter translators" - an array with one entry
  per scpi parameter (see scpi.h), for which each entry
  consists of the parameter_translator structure
  described below. Each descendant of AWG_FY must supply
  a virtual get_pt() function to provide access to its
  particular translator table.
  
********************************************************/

class AWG_FY : public AWG_Server
{
  public:

    AWG_FY ( uint32_t retries = 0 )
      : AWG_Server(retries)
      {}

    virtual bool    set ( uint32_t channel, uint32_t param_id, double value );
    virtual double  get ( uint32_t channel, uint32_t param_id );

  protected:

    virtual uint32_t  translate_wave ( uint32_t wave, bool direction );

    virtual param_translator *  get_pt () = 0;
};

#endif