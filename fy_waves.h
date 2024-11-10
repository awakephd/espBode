#ifndef FY_WAVES_H
#define FY_WAVES_H

/*!
  @file   fy_waves.h
  @brief  Declaration and definition of the
          fy::wave_types enumeration.

  This enumeration is not needed or used by the current
  espBode implementation. Since the Bode plot only uses a
  sine wave, any request to set wave type defaults to
  using 0 (Sine wav for the FY series). However, this
  enumeration is included for the sake of possible future
  extensions of functionality.
*/

/*!
  @brief  The fy namespace is used for identifiers specific to the FY-series of AWGs.
*/
namespace fy {

  /*!
    @brief  Enumeration of the basic wave form ids for
            the FeelTech FY-series AWGs.

    For the FY-series of AWGs, wave form type is indicated by
    an integer, included as part of the write command or
    returned after a read command. For example, to set the AWG
    to output a Rectangle wave on the main channel, the
    following command will be send to the FY AWG over its
    USB/serial communication port: WMW2 (where 2 = the
    Rectangle id from the wave_types enumeration).
  */
  enum wave_types
  {
    Sine        = 0,    ///< Sine wave
    Square      = 1,    ///< Square wave (fixed 50% duty cycle)
    Rectangle   = 2,    ///< Rectangle wave (same as square but with variable duty cycle)
    Trapezoid   = 3,    ///< Same as Rectangle but with adjustable rise and fall times
    CMOS        = 4,
    Adj_Pulse   = 5,
    DC          = 6,
    TRGL        = 7,
    Ramp        = 8,
    NegRamp     = 9,
    Stair_TRGL  = 10,
    Stairstep   = 11,
    NegStair    = 12,
    PosExponen  = 13,
    NegExponen  = 14,
    P_Fall_Exp  = 15,
    N_Fall_Exp  = 16,
    PosLogarit  = 17,
    NegLogarit  = 18,
    P_Fall_Log  = 19,
    N_Fall_Log  = 20,
    P_Full_Wave = 21,
    N_Full_Wave = 22,
    P_Half_Wave = 23,
    N_Half_wave = 24,
    Lorenzt_Pu  = 25,
    Multitone   = 26,
    Random_Noi  = 27,
    ECG         = 28,
    Trapezoid2  = 29,
    Sinc_Pulse  = 30,
    impulse     = 31,
    AWGN        = 32,
    AM          = 33,
    FM          = 34,
    Chirp       = 35,
    Impulse     = 36,

    wave_count  = 37  ///< The number of wave forms identified in the enumeration
  };
}

#endif