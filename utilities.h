#ifndef UTILITIES_H
#define UTILITIES_H

/*!
  @file   utilities.h
  @brief  Brief classes and functions used by other parts of
          the espBode program.
*/

#include <stdint.h>
#include "Streaming.h"

#ifdef USE_LED

  /*!
    @brief  Blink the built-in led.

    This function allows visual user feedback by blinking the built-in
    LED a selected number of times at a selected interval (specified
    in milliseconds). This function is declared here and defined in
    utilities.cpp only if USE_LED is defined.

    @param  count           Number of times to blink the LED
    @param  interval_in_ms  Duration in milliseconds of on and off period
  */
  void blink ( int count, int interval_in_ms );

#endif

/*!
  @brief  Wait for serial input to become available.

  @param  b_consume If true, read all available input and discard it.
*/
inline void wait_for_serial ( bool b_consume = false )
{
  // wait until something is available

  while ( ! Serial.available() );

  // if b_consume is set, clear the Serial input

  if ( b_consume )
  {
    while ( Serial.available() )
    {
      Serial.read();
    }
  }
}

/*!
  @brief  A quick way to get a power of 10 up to +/-9.

  Note that this function uses integer multiplication, then
  converts the result to double-precision floating point.

  @param  p   The exponent to use (range: -9 to 9)

  @return If -9 <= p <= 9, return 10^p; else return 1
*/
inline double pow10 ( int p )
{
  if ( p < -9 || p > 9 )
  {
    return 1;
  }

  uint32_t  val = 1;
  int       n = std::abs(p);

  // Use integer multiplication for speed

  for ( int i = 0; i < n; i++ )
  {
    val *= 10;
  }

  if ( p < 0 )
  {
    return (double)1 / (double)val;
  }
  else
  {
    return (double)val;
  }
}

/*!
  @brief  Swap the bytes of a 4-byte integer.

  std::byteswap is available in C++23. However,
  the Arduino ESP8266 package does not support
  C++23. Therefore, we must supply the function.

  @param  data  4-byte integer for which the bytes should be swapped

  @result The 4-byte integer with its bytes swapped
*/
inline uint32_t byteswap ( uint32_t data )
{
  uint32_t  result;

  result = ( ( data & 0x000000ff ) << 24 );
  result |= ( ( data & 0x0000ff00 ) << 8 );
  result |= ( ( data & 0x00ff0000 ) >> 8 );
  result |= ( ( data & 0xff000000 ) >> 24 );

  return result;
}

/*!
  @brief  Manages storage and conversion of 4-byte big-endian data.

  The WiFi packets used by RPC_Server and VXI_Server transmit
  data in big-endian format; however, the ESP8266 and C++
  use little-endian format. When a packet is read into a buffer
  which has been described as a series of big_endian_32_t members,
  this class allows automatic conversion from the big-endian
  packet data to a little-endian C++ variable and vice-versa.
*/
class big_endian_32_t
{
  private:

    uint32_t  b_e_data;   ///< Data storage is treated as uint32_t, but the data in this member is actually big-endian

  public:

    /*!
      @brief  Constructor takes a uint32_t and stores it in big-endian form.

      The presence of this constructor allows assignment of a little-endian value
      (e.g., a C++ variable) to a big_endian_32_t field in the WiFi packet with
      automatic conversion.

      @param  data  little-endian data to be converted and stored as big-endian
    */
    big_endian_32_t ( uint32_t data )
      { b_e_data = byteswap(data); }

    /*!
      @brief  Implicit or explicit conversion to little-endian uint32_t.

      @return Little-endian equivalent of big-endian data stored in m_data
    */
    operator uint32_t ()
      { return byteswap(b_e_data); }
};

/*!
  @brief  4-byte integer that cycles through a defined range.

  The cyclic_uint32_t class allows storage, retrieval,
  and increment/decrement of a value that must cycle
  through a constrained range. When an increment or
  decrement exceeds the limits of the range, the
  value cycles to the opposite end of the range.
*/
class cyclic_uint32_t
{
  private:

    uint32_t  m_data;
    uint32_t  m_start;
    uint32_t  m_end;

  public:

    /*!
      @brief  The constructor requires the range start and end.

      Initial value may optionally be included as well; if it is
      not included, the value will default to the start of the range.
    */
    cyclic_uint32_t ( uint32_t start, uint32_t end, uint32_t value = 0 )
      { m_start = start < end ? start : end;
        m_end = start < end ? end : start;
        m_data = value >= m_start && value <= m_end ? value : m_start; }

    /*!
      @brief  Cycle to the previous value.

      If the current value is at the start of the range, the
      "previous value" will loop around to the end of the range.
      Otherwise, the "previous value" is simply the current
      value - 1.

      @return The previous value.
    */
    uint32_t  goto_prev ()
      { m_data = m_data > m_start ? m_data - 1 : m_end;
        return m_data; }

    /*!
      @brief  Cycle to the next value.

      If the current value is at the end of the range, the
      "next value" will loop around to the start of the range.
      Otherwise, the "next value" is simply the current value + 1.

      @return The next value.
    */
    uint32_t  goto_next ()
      { m_data = m_data < m_end ? m_data + 1 : m_start;
        return m_data; }

    /*!
      @brief  Pre-increment operator, e.g., ++port.

      @return The next value (the value after cyling forward).
    */
    uint32_t  operator ++ ()
      { return goto_next(); }

    /*!
      @brief  Post-increment operator, e.g., port++.

      @return The current value (the value before cyling forward).
    */
    uint32_t  operator ++ (int)
      { uint32_t  temp = m_data;
        goto_next();
        return temp; }

    /*!
      @brief  Pre-decrement operator, e.g., --port.

      @return The previous value (the value after cyling backward).
    */
    uint32_t  operator -- ()
      { return goto_prev(); }

    /*!
      @brief  Post-decrement operator, e.g., port--.

      @return The current value (the value before cyling backward).
    */
    uint32_t  operator -- (int)   // postfix version
      { uint32_t  temp = m_data;
        goto_prev();
        return temp; }

    /*!
      @brief  Allows conversion to uint32_t.

      Example:
      @code
        regular_uint32_t = cyclic_uint32_t_instance();
      @endcode

      @return The current value.
    */
    uint32_t  operator () ()
      { return m_data; }

    /*!
      @brief  Implicit conversion or explicit cast to uint32_t.

      Example:
      @code
        regular_uint32_t = (uint32_t)cyclic_uint32_t_instance;
      @endcode

      @return The current value.
    */
   	operator uint32_t ()
 	  	{ return m_data; }
};

/*!
  @brief  Converts cyclic_uint32_t to uint32_t for streaming.

  The following allows "transparent" use of a cyclic_uint32
  in a stream operation without having to explicitly cast
  it; e.g., instead of having to write
  @code
    Serial << (uint32_t)cyclic_uint32_t_instance;   or
    Serial << cyclic_uint32_instance();
  @endcode
  we can just write\n
  @code
    Serial << cyclic_uint32_val;
  @endcode

  @param  stream  Reference to a Print object to which to stream the value
  @param  value   The cyclic_uint32_t object to output to the stream

  @return Reference to the Print object
*/
inline Print& operator << ( Print & stream, cyclic_uint32_t & value )
{
  stream << (uint32_t)value;

  return stream;
}

#endif