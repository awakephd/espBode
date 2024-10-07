#include "debug.h"

#ifdef INSTANTIATE_DEBUG
  DEBUG     Debug;
#endif

/***************************************************
  The Dump member function outputs the contents of
  a buffer in both hex and (where valid) ascii via
  the DEBUG output channel.
****************************************************/

DEBUG & DEBUG::Dump ( uint8_t * buffer, int len )
{
  char      ascii_buffer[17];
  char *    ab;
  uint8_t   c;
  int       i, j;
  
  ascii_buffer[16] = 0;

  while ( len > 0 ) {
    ab = ascii_buffer;

    if ( len < 16 ) {
      ascii_buffer[len] = 0;
    }

    for ( i = 0; i < 16; i += 4 ) {
      for ( j = 0; j < 4; j++ ) {      
        if ( i + j < len ) {
          c = *buffer;
          buffer++;

          printf("%02x ", c);

          *ab = ( c < 0x20 ? '.' : ( c > 0x7f ? '.' : c ));
          ab++;
        } else {
          print("   ");
        }
      }

      print(" ");
    }

    println(ascii_buffer);
    len -= 16;
  }
  
  return *this;
}

/**************************************************************
  The following member function is an override of the pure
  virtual write() function in the Print class. This is the
  function on which prinf(), printf(), and other Print output
  functions depend on. If the current output level > the
  current debug level, we simply discard the character.
  Otherwise, to improve performance, we store the character
  in the buffer; when either the charactger = '\n' or the
  buffer is full, we flush the buffer out to Serial or Telnet.
**************************************************************/

size_t DEBUG::write ( uint8_t byte )
{
  /*  If the current output level is > the current debug level,
      discard byte and return 0  */

  if ( m_output_level > m_debug_level ) {
    return 0;

  /*  Otherwise, store the byte in the buffer. If the byte =
      '\n' or the buffer is full, flush it to the current
      output channel.  */

  } else {
    buffer[index++] = byte;

    if ( byte == '\n' || index >= DEBUG_BUFFER_SIZE ) {
      flush();
    }

    /*  indicate the successful "write" of the character  */

    return 1;
  }
}


/**************************************************************
  The following member function is an override of the virtual
  flush() function in Print. This function sends the contents
  of the buffer, if any, either to Serial or to Telnet and
  resets the buffer.
**************************************************************/

void DEBUG::flush()
{
  if ( index > 0 ) {
    if ( m_channel == VIA_SERIAL ) {
      Serial.write(buffer,index);
    } else {
      Telnet.write(buffer,index);
  }

    index = 0;
  }
}
