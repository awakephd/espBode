#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>


inline uint32_t byteswap ( uint32_t data )
{
  uint32_t  result;

  result = ( ( data & 0x000000ff ) << 24 );
  result |= ( ( data & 0x0000ff00 ) << 8 );
  result |= ( ( data & 0x00ff0000 ) >> 8 );
  result |= ( ( data & 0xff000000 ) >> 24 );

  return result;
}


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


class cycle
{
  private:

    uint32_t  m_data;
    uint32_t  m_start;
    uint32_t  m_end;

  public:

    cycle ( uint32_t i_start, uint32_t i_end, uint32_t i_data = 0 )
      { m_start = i_start < i_end ? i_start : i_end;
        m_end = i_start < i_end ? i_end : i_start;
        m_data = i_data >= m_start && i_data <= m_end ? i_data : m_start; }

    uint32_t  get_next ()
      { m_data = m_data < m_end ? m_data + 1 : m_start;
        return m_data; }

    uint32_t  operator ++ ()      // prefix version
      { return get_next(); }

    uint32_t  operator ++ (int)   // postfix version
      { uint32_t  temp = m_data;
        get_next();
        return temp; }
 
   	operator uint32_t ()
 	  	{ return m_data; }
};

#endif