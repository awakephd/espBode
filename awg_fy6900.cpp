#include "awg_fy6900.h"

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