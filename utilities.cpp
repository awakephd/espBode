/*!
  @file   utilities.cpp
  @brief  Defines functions and methods declared in utilities.h.
*/

#include <Arduino.h>
#include "led_config.h"

#ifdef USE_LED

void blink ( int count, int interval )
{
  for ( int i = 0; i < count; i++ ) {
    digitalWrite(LED_BUILTIN, LED_ON);
    delay(interval);
    digitalWrite(LED_BUILTIN, LED_OFF);
    delay(interval);
  }
}

#endif