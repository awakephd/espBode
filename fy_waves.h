#ifndef FY_WAVES_H
#define FY_WAVES_H

/*** FY_WAVES **********************************

  The following enum identifies the basic wave
  form ids for the FeelTech AWGs.

**********************************************/

namespace fy {

  enum wave_types
  {
    Sine        = 0,
    Square      = 1,
    Rectangle   = 2,
    Trapezoid   = 3,
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

    wave_count  = 37
  };
}

#endif