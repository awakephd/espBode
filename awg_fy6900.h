#ifndef AWG_FY6900_H
#define AWG_FY6900_H

#include "awg_fy.h"

/*** AWG_FY6900 ************************************

  This class implements the details needed to
  carry out the AWG_FY get and set methods for the
  newer firmware versions of the FY6900.

***************************************************/

class AWG_FY6900 : public AWG_FY
{
  public:

    AWG_FY6900 ( uint32_t retries = 0 )
      : AWG_FY(retries)
      {}

  protected:

    virtual param_translator *  get_pt ();
};

#endif