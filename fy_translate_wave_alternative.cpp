/*!
  @file   fy_translate_wave_alternative.cpp
  @brief  A "full" implementation of the translate_wave function.

  For the sake of completeness, the code in this file implements
  the full range of wave shapes for the FY series of AWGs. In terms
  of a Bode plot, the only wave that is needed is a Sine wave, so
  the default code contained in the awg_fy.cpp file is very simple,
  saving space and complexity. If for some reasong the full range of
  waves is needed, one can define USE_ALTERNATIVE_TRANSLATE_WAVE in
  the awg_fy.h file; this will cause the code below to be used instead
  of the default, "simple" translate_wave code.
*/

#ifdef USE_ALTERNATIVE_TRANSLATE_WAVE

/*!
  @brief  The FeelTech wave types indexed according
          to the matching Siglent wave types.

  The index of each entry in the following array
  matches the number of the equivalent Siglent wave
  form. A value of -1 means there is no FeelTech wave
  form to match the corresponding Siglent wave form.
*/
int8_t wave_table [] = { fy::Sine,          // 0 = Sine 
                         fy::Random_Noi,    // 1 = Noise
                         fy::Stairstep,     // 2 = StairUp
                         fy::NegStair,      // 3 = StairDn
                         fy::Stair_TRGL,    // 4 = Stairud
                         fy::Adj_Pulse,     // 5 = Ppulse
                         -1,                // 6 = Npulse
                         fy::Trapezoid,     // 7 = Trapezia
                         fy::Ramp,          // 8 = Upramp
                         fy::NegRamp,       // 9 = Dnramp
                         fy::NegExponen,    // 10 = Exp_fall
                         fy::PosExponen,    // 11 = Exp_rise
                         fy::NegLogarit,    // 12 = Logfall
                         fy::PosLogarit,    // 13 = Logrise
                         -1,                // 14 = Sqrt
                         -1,                // 15 = Root3
                         -1,                // 16 = X_2
                         -1,                // 17 = X_3
                         fy::Sinc_Pulse,    // 18 = Sinc
                         -1,                // 19 = Gaussian
                         -1,                // 20 = Dlorentz
                         -1,                // 21 = Haversine
                         fy::Lorenzt_Pu,    // 22 = Lorentz
                         -1,                // 23 = Gauspuls
                         -1,                // 24 = Gmonopuls
                         -1,                // 25 = Tripuls
                         fy::ECG,           // 26 = Cardiac
                         -1,                // 27 = Quake
                         fy::Chirp,         // 28 = Chirp
                         fy::Multitone,     // 29 = Twotone
                         -1,                // 30 = Snr
                         -1,                // 31 = Hamming
                         -1,                // 32 = Hanning
                         -1,                // 33 = Kaiser
                         -1,                // 34 = Blackman
                         -1,                // 35 = Gausswin
                         fy::TRGL,          // 36 = Triang
                         -1,                // 37 = Harris
                         -1,                // 38 = Bartlett
                         -1,                // 39 = Tan
                         -1,                // 40 = Cot
                         -1,                // 41 = Sec
                         -1,                // 42 = Csc
                         -1,                // 43 = Asin
                         -1,                // 44 = Acos
                         -1,                // 45 = Atan
                         -1,                // 46 = Acot
                         fy::Rectangle      // 47 = Square
                       };

uint32_t AWG_FY::translate_wave ( uint32_t wave, bool direction )
{
  uint32_t  ret_wave;

  if ( direction == siglent::from_sig )
  {
    if ( wave < siglent::wave_count )
    {
      ret_wave = ( wave_table[wave] == -1 ) ? fy::Sine : wave_table[wave];
    }
    else
    {
      ret_wave = fy::Sine;
    }
  }
  else
  {
    ret_wave = siglent::Sine;

    for ( int i = 0; i < siglent::wave_count; i++ )
    {
      if ( wave_table[i] == wave ) {
        ret_wave = i;
        break;
      }
    }
  }

  return ret_wave;
}

#endif
