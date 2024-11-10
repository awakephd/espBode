#ifndef AWG_FY6900_H
#define AWG_FY6900_H

/*!
  @file   awg_fy6900.h
  @brief  Declares the AWG_FY6900 class.
*/

#include "awg_fy.h"

/*!
  @brief  Supplies the translation table needed for the
          AWG_FY class to serve FY6900 AWGs with recent
          (>= 1.4) firmware.
*/
class AWG_FY6900 : public AWG_FY
{
  public:

    /*!
      @brief  Constructor only passes along the optional retries parameter to the AWG_FY constructor.
    */
    AWG_FY6900 ( uint32_t retries = 0 )
      : AWG_FY(retries)
      {}

  protected:

    /*!
      @brief  Supplies a pointer to the translation table for FY6900 AWGs with recent (>= 1.4) firmware.

      @return A pointer to the translation table.
    */
    virtual param_translator *  get_pt ();
};

#endif