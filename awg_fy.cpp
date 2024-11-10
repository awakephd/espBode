/*!
  @file   awg_fy.cpp
  @brief  Defines the methods and supporting tables used by the AWG_FY class.
*/

#include "awg_fy.h"
#include "fy_waves.h"
#include "siglent_waves.h"
#include "utilities.h"
#include "Streaming.h"
#include "debug.h"

/*!
  @brief  Letters used to indicate FeelTech parameters.

  The table contains the FeelTech codes (letters) indexed
  to match the scpi parameters (see scpi::parameters and
  scpi::parameter_id) which are passed by get() or set().
*/
char  fy_codes[] =  { 'N',    ///< 0 = OUTP (off)
                      'N',    ///< 1 = OUTP (on)
                      'W',    ///< 2 = WVTP
                      'F',    ///< 3 = FRQ
                      'A',    ///< 4 = AMP
                      'O',    ///< 5 = OFST
                      'P'     ///< 6 = PHSE
                    };   

/*!
  @brief  Letters used to indicate FeelTech channels.

  The table contains the FeelTech codes (letters) that
  refer to the channels of the matching index; e.g.,
  fy_channels[1] = 'M', the letter that indicates
  channel 1.
*/
char  fy_channels[] = { 'M',    ///< shouldn't encounter channel = 0, but just in case, set to main channel
                        'M',    ///< Channel 1
                        'F',    ///< Channel 2
                      };   

bool AWG_FY::set ( uint32_t channel, uint32_t param_id, double value )
{
  param_translator *  pt = get_pt();
  int                 retries = retry();
  char                command[] = "WMF";
  double              p10, set_value;
  bool                b_validate, b_ok = true;
  int                 width = 0, precision = 0;

  /*  Test channel and parameter to make sure they are valid.
      Note that channel is 1-based, not 0-based.  */

  if ( channel > channels() || param_id > scpi::parameter_count )
  {
    return false;     // invalid channel or parameter
  }

  command[1] = fy_channels[channel];
  command[2] = fy_codes[param_id];

  if ( param_id == scpi::WAVE )
  {
    set_value = translate_wave ( value, siglent::from_sig );
  }
  else
  {
    width = pt[param_id].set_width;
    precision = pt[param_id].set_precision;

    p10 = pow10(precision);
    value = ((uint64_t)(value * p10 + 0.5)) / p10;  // limit value to set_precision

    p10 = pow10(pt[param_id].set_exponent);
    set_value = value * p10;                          // adjust value to desired units
  }

  b_validate = ( retries > 0 );

  /*!
    @todo Separate the serial communication into an asynchronous operation
          with 1) timeout and 2) ability to be cancelled by external command
          such as abort or destroy_link.
  */

  do
  {
    Serial << command;
    Debug.Serial_IO() << command;
  
    switch ( pt[param_id].set_type )
    {
      case pt_BOOL:

        set_value = ( value == 0 ) ? 0 : 1;

        // fall through to send this as an INT

      case pt_INT:

        if ( width )
        {
          Serial << _WIDTH((long int)(set_value), width);
          Debug.Serial_IO() << _WIDTH((long int)(set_value), width);
        }
        else
        {
          Serial << (long int)(set_value);
          Debug.Serial_IO() << (long int)(set_value);
        }

        break;

      case pt_DOUBLE:

        if ( width )
        {
          Serial << _WIDTH(_FLOAT(set_value,precision),width);
          Debug.Serial_IO() << _WIDTH(_FLOAT(set_value,precision),width);
        }
        else if ( precision )
        {
          Serial << _FLOAT(set_value,precision);
          Debug.Serial_IO() << _FLOAT(set_value,precision);
        }
        else
        {
          Serial << set_value;
          Debug.Serial_IO() << set_value;
        }

        break;

      default:

        break;
    }

    Serial << "\n";     // complete the line
    Debug.Serial_IO() << "\n";

    /*  wait for the AWG to respond (it should send back a single '\n')
        and discard the response. This will also clear any left-over
        input from the AWG.
    */

    wait_for_serial(true);

    if ( b_validate )
    {
      b_ok = ( value == get(channel, param_id) );
    }
  }
  while ( ! b_ok && retries-- > 0 );

  if ( ! b_ok ) {
    Debug.Error() << "Unable to verify " << scpi::parameters[param_id] << "\n";
  }
  return b_ok;
}

double AWG_FY::get ( uint32_t channel, uint32_t param_id )
{
  param_translator *  pt = get_pt();
  char                command[] = "RMF\n";
  char                response[awg_response_length+1];
  double              p10, value = -1.23;
  int                 len;

  /*  Test channel and parameter to make sure they are valid.
      Note that channel is 1-based, not 0-based.
  */

  if ( channel > channels() || param_id > scpi::parameter_count )
  {
    return false;     // invalid channel or parameter
  }

  command[1] = fy_channels[channel];
  command[2] = fy_codes[param_id];

  p10 = pow10(pt[param_id].get_exponent);

  Serial << command << "\n";
  Debug.Serial_IO() << command << "\n";

  // wait until the AWG responds

  wait_for_serial();

  len = Serial.readBytesUntil('\n', response, awg_response_length);

  response[len] = 0;

  Debug.Serial_IO() << response << "\n";

  sscanf(response, "%lf", &value);

  value *= p10;

  if ( pt[param_id].get_type == pt_BOOL )
  {
    value = ( value == 0 ) ? 0 : 1;
  }

  return value;
}

#ifdef USE_ALTERNATIVE_TRANSLATE_WAVE

  #include "fy_translate_wave_alternative.cpp"

#else

  uint32_t AWG_FY::translate_wave ( uint32_t wave, bool direction )
  {
    return ( direction == siglent::from_sig ) ? fy::Sine : siglent::Sine;
  }

#endif