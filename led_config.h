#ifndef LED_CONFIG_H
#define LED_CONFIG_H

/*!
  @file   led_config.h
  @brief  Configuration details for the built-in LED.
*/

/*!
  @brief  Enable use of the built-in LED.

  On the ESP-01 (not the -S version), the built-in LED
  interferes with Serial, and therefore should not be
  used. Comment out the #define to prevent use of the LED.
*/
#define USE_LED

const int LED_ON = LOW;     ///< The value to write to turn on the LED
const int LED_OFF = HIGH;   ///< The value to write to turn off the LED

#endif
