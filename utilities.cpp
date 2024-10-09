#include <Arduino.h>
#include "espBode.h"


/*** blink() ****************************************************

A small utility function to allow visual user feedback by
blinking the built-in LED a selected number of times at a
selected interval (specified in milliseconds). This function
is only defined if USE_LED is defined.

*****************************************************************/

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