/*!
  @file   awg_fy6900.cpp
  @brief  Defines the methods and translation table of the AWG_FY6900 class.
*/

#include "awg_fy6900.h"

/*!
  @brief  The translation table needed for FY6900 AWGs with recent (>= 1.4) firmware.

  The table is based on the param_translator structure defined in awg_fy.h. The
  index of the row matches the id of the parameter (see scpi::param_id). Each
  entry indicates the type, exponent, precision, and width of values to send,
  and the type and exponenet of values received.

  Recent firmware versions of the FY6900 AWG mostly use floating point (double)
  values to set frequency, offset, phase, and amplitude. When reading values,
  frequency is returned as a floating point number, but ammplitude, offset, and
  phase are returned as integers multiplied by 10^4 (amplitude) or 10^3 (offset
  and phase). On/Off are represented by integers, where 0 = off and non-zero = on;
  wave type is represented by an integer representing the id of the wave type
  (see fy::wave_types).
*/
param_translator  pt6900[] =
  { { pt_BOOL, 0, 0, 0, pt_BOOL, 0 },      // OFF
    { pt_BOOL, 0, 0, 0, pt_BOOL, 0 },      // ON
    { pt_INT, 0, 0, 0, pt_INT, 0 },        // WVTP
    { pt_DOUBLE, 0, 6, 0, pt_DOUBLE, 0 },  // FRQ
    { pt_DOUBLE, 0, 4, 0, pt_INT, -4 },    // AMP
    { pt_DOUBLE, 0, 3, 0, pt_INT, -3 },    // OFST
    { pt_DOUBLE, 0, 3, 0, pt_INT, -3 }     // PHSE
  };

param_translator * AWG_FY6900::get_pt ()
{
  return pt6900;
}