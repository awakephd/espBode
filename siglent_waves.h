#ifndef SIGLENT_WAVES_H
#define SIGLENT_WAVES_H

/*!
  @file   siglent_waves.h
  @brief  Declaration and definition of the
          siglent::BSWV_types enumeration.

  This enumeration is actually not needed o4 used by the
  current espBode implementation. Since the Bode plot only
  uses a sine wave, any SCPI command for wave type is
  assumed to call for a sine wave. This enumeration is
  included in case of possible future extension of
  functionality.
*/

/*!
  @brief  The siglent namespace is used for identifiers specific to Siglent SCPI commands.
*/
namespace siglent {

  /*!
    @brief  Enumeration of the BSWV wave types
            used by Siglent AWGs and oscilloscopes.

    For Siglent instruments, wave form is expressed in an SCPI
    command by a string (e.g., "SINE") rather than by a
    number. It is very likely that the only one we care about
    is the sine wave. However, if it ever becomes necessary to
    translate the Siglent waveform into the waveform usedby
    one of the budget AWGs that espBode-2 covers, it will be
    helpful to have an integer code to represent the Siglent
    waveforms. This enumeration is based on the list provided
    in the WGEN section of the SDS Series Digital Oscilloscope
    Programming Guide EN11G. Note that we are only listing the
    BaSic WaVeform types; we are ignoring the ARbitrary WaVforms.
  */
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

    wave_count  = 48    ///< The number of Siglent waveform ids
  };

  // constants that can be used in translating to/from the Siglent waveform ids

  const bool from_sig = true;   ///< true indicates that the tranlsation is from the Siglent to the discount AWG
  const bool to_sig = false;    ///< false indicates that the translation is from the discount AWG to the Siglent
};

#endif