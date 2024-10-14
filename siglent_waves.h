#ifndef SIGLENT_WAVES_H
#define SIGLENT_WAVES_H

/*** SIGLENT_WAVES ****************************

  The following enum identifies the basic wave
  forms recognized by the Siglent AWGs. It is
  very likely that the only one we care about is
  the sine wave ... but if we need it, here is
  the list provided in the WGEN section of the
  SDS Series Digital Oscilloscope Programming
  Guide EN11G. Note that we are only listing
  the BaSic WaVeform types; we will ignore the
  ARbitrary WaVforms.

**********************************************/

namespace siglent {

  enum BSWV_types
  {
    Sine        = 0,
    Noise       = 1,
    StairUp     = 2,
    StairDn     = 3,
    Stairud     = 4,
    Ppulse      = 5,
    Npulse      = 6,
    Trapezia    = 7,
    Upramp      = 8,
    Dnramp      = 9,
    Exp_fall    = 10,
    Exp_rise    = 11,
    Logfall     = 12,
    Logrise     = 13,
    Sqrt        = 14,
    Root3       = 15,
    X_2         = 16,
    X_3         = 17,
    Sinc        = 18,
    Gaussian    = 19,
    Dlorentz    = 20,
    Haversine   = 21,
    Lorentz     = 22,
    Gauspuls    = 23,
    Gmonopuls   = 24,
    Tripuls     = 25,
    Cardiac     = 26,
    Quake       = 27,
    Chirp       = 28,
    Twotone     = 29,
    Snr         = 30,
    Hamming     = 31,
    Hanning     = 32,
    Kaiser      = 33,
    Blackman    = 34,
    Gausswin    = 35,
    Triang      = 36,
    Harris      = 37,
    Bartlett    = 38,
    Tan         = 39,
    Cot         = 40,
    Sec         = 41,
    Csc         = 42,
    Asin        = 43,
    Acos        = 44,
    Atan        = 45,
    Acot        = 46,
    Square      = 47,

    wave_count  = 48
  };

  // variables that can be used in translating to/from to above

  const bool from_sig = true;
  const bool to_sig = false;
};

#endif