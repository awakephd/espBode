#ifndef AWG_FY_H
#define AWG_FY_H

/*!
  @file   awg_fy.h
  @brief  Declaration of the AWG_FY class and supporting structures, variables, and enumerations.
*/

#include "awg_server.h"

const int  awg_response_length = 20;  ///< Maximum length of any line received from an FY-series AWG

/*!
  @brief  The structure used to translate parameters for sending to and receiving from FY-series AWGs.

  The AWG_FY class makes use of a "translation table," consisting of one row per parameter
  to be translated (see the scpi::parameters table and scpi::parameter_id enumeration). Each
  row must consist of the param_translator structure. The structure identifies the type of
  value used when setting and getting a parameter, along with its expected multiplier (power of
  10). When setting parameters, precision and total width can also be supplied, or left as 0
  to accept the default. Note that the AWG_FY class does not provide a default table; instead,
  it expects a descendant class to override the pure virtual get_pt() method to provide the
  table suitable for a specific variant of the FY-series AWGs.
*/
struct param_translator
{
  uint8_t   set_type;       ///< type of value to send to AWG; see param_translator_types
  int8_t    set_exponent;   ///< multiply value by 10^exponent before sending
  uint8_t   set_precision;  ///< if type = double, how many decimal places to include
  uint8_t   set_width;      ///< if width != 0, indicates need to zero-fill to achieve width
  uint8_t   get_type;       ///< type of value read from AWG; see param_translator_types
  int8_t    get_exponent;   ///< value read from AWG must be multiplied by 10^exponent
};

/*!
  @brief  The types of values used to send values to or receive values from the AWG.
*/
enum  param_translator_types
{
  pt_BOOL   = 0,    ///< the value will be zero = false or non-zero = true
  pt_INT    = 1,    ///< the value will be 32-bit integer
  pt_DOUBLE = 2     ///< the value will be double-precision floating point
};

/*!
  @brief  Provides the interface common to all FY-series AWGs.

  The FeelTech AWGs all use a similar, but not entirely identical,
  command structure for getting and setting parameters. The variations
  can be described by an array of param_translator entries, with
  one entry per scpi parameter (see the scpi::parameters table and
  scpi::parameter_id enumeration). Each descendant of AWG_FY must
  override the virtual get_pt() member function to provide access to
  its particular translation table.
*/
class AWG_FY : public AWG_Server
{
  public:

    /*!
      @brief  Constructor merely passes the optional retries setting to the AWG_Server constructor.
    */
    AWG_FY ( uint32_t retries = 0 )
      : AWG_Server(retries)
      {}

    /*!
      @brief  Format and send a command to set the specified AWG parameter.

      The set() method will use get_pt() to retrieve the translation table,
      and will use the appropriate entry in that table to determine how to
      format the value to be sent.

      @param  channel   1 or 2 to indicate Channel 1 or Channel 2
      @param  param_id  The id of the parameter to be set (see scpi::parameter_id)
      @param  value     The value to which to set the specified parameter

      @return True = value was successfully set.
    */
    virtual bool    set ( uint32_t channel, uint32_t param_id, double value );

    /*!
      @brief  Send a command to read the specified AWG parameter and interpret the result.

      The set() method will use get_pt() to retrieve the translation table,
      and will use the appropriate entry in that table to determine how to
      format the value to be sent.

      @param  channel   1 or 2 to indicate Channel 1 or Channel 2
      @param  param_id  The id of the parameter to be set (see scpi::parameter_id)

      @return The value read from the AWG for the specified parameter.
    */
    virtual double  get ( uint32_t channel, uint32_t param_id );

  protected:

    /*!
      @brief  Translate between FY wave id and Siglent wave id.

      The default implementation simply uses Sine wave for both
      Siglent and FY AWG. If there is a need to use a larger range of
      wave types, this method can be overridden in a descendant class,
      or an alternative translator can be included by way of a
      #define in the awg_fy.cpp file.

      @param  wave      The id of the wave form to be translated.
      @param  direction siglent::from_sig or siglent::to_sig

      @return The matching id for the Siglent or the FY AWG, depending on direction.
    */
    virtual uint32_t  translate_wave ( uint32_t wave, bool direction );

    /*!
      @brief  Provide a pointer to the param_translator table used by set() and get().

      This is a pure virtual function in the AWG_FY class; it must be overridden by
      a descenant class to provide the appropriate table.

      @return A pointer to the translation table.
    */
    virtual param_translator *  get_pt () = 0;
};

/*!
  @def    USE_ALTERNATIVE_TRANSLATE_WAVE

  @brief  Indicate whether to translate the wave
          parameter fully or simply.
          
  If USE_ALTERNATIVE_TRANSLATE_WAVE is not defined,
  a simple, default version of translate_wave will
  be used which simply returns a sine wave. If this
  macro is defined, the alternate, "full" implementation
  of translate_wave will be used. See the file
  fy_translate_wave_alternative.cpp for detail.
*/

//#define USE_ALTERNATIVE_TRANSLATE_WAVE

#endif