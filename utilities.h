#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

/*** blink() ****************************************************

A small utility function to allow visual user feedback by
blinking the built-in LED a selected number of times at a
selected interval (specified in milliseconds). This function
is declared here and defined in utilities.cpp only if the LED
is used (signaled by USE_LED being defined).

*****************************************************************/

#ifdef USE_LED
  void blink ( int count, int interval );
#endif

/************************************************
  std::byteswap is available in C++23. However,
  the Arduino ESP8266 package does not support
  C++23. Therefore, we must supply the function.
************************************************/

inline uint32_t byteswap ( uint32_t data )
{
  uint32_t  result;

  result = ( ( data & 0x000000ff ) << 24 );
  result |= ( ( data & 0x0000ff00 ) << 8 );
  result |= ( ( data & 0x00ff0000 ) >> 8 );
  result |= ( ( data & 0xff000000 ) >> 24 );

  return result;
}

/****************************************************
  The big_end_uint32 class stores 4-byte data in
  big-endian format; it handles automatic conversion
  to the little-endian format used by the ESP8266
  and C++.
***************************************************/

class big_end_uint32
{
  private:

    uint32_t  m_data;

  public:

    big_end_uint32 ( uint32_t input )
      { m_data = byteswap(input); }

    operator uint32_t ()
      { return byteswap(m_data); }

    uint32_t  as_is ()
      { return m_data; }
};

/****************************************************
  The cyclic_uint32 class allows storage, retrieval,
  and increment/decrement of a value that must cycle
  through a constrained range. When an increment or
  decrement exceeds the limits of the range, the
  value cycles to the opposite end of the range.
*****************************************************/

class cyclic_uint32
{
  private:

    uint32_t  m_data;
    uint32_t  m_start;
    uint32_t  m_end;

  public:

    cyclic_uint32 ( uint32_t start, uint32_t end, uint32_t data = 0 )
      { m_start = start < end ? start : end;
        m_end = start < end ? end : start;
        m_data = data >= m_start && data <= m_end ? data : m_start; }

    uint32_t  get_prev ()
      { m_data = m_data > m_start ? m_data - 1 : m_end;
        return m_data; }

    uint32_t  get_next ()
      { m_data = m_data < m_end ? m_data + 1 : m_start;
        return m_data; }

    uint32_t  operator ++ ()      // prefix version
      { return get_next(); }

    uint32_t  operator ++ (int)   // postfix version
      { uint32_t  temp = m_data;
        get_next();
        return temp; }

    uint32_t  operator -- ()      // prefix version
      { return get_prev(); }

    uint32_t  operator -- (int)   // postfix version
      { uint32_t  temp = m_data;
        get_prev();
        return temp; }
 
    uint32_t  operator () ()
      { return m_data; }

   	operator uint32_t ()
 	  	{ return m_data; }
};

#endif