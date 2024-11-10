#include "debug.h"

/*!
  @file   debug.cpp
  @brief  Definitions of the methods the DUMP class
*/

#ifdef INSTANTIATE_DEBUG
  DEBUG     Debug;    ///< Global instance of DEBUG, instantiated in debug.cpp if INSTANTIATE_DEBUG is defined
#endif

DEBUG & DEBUG::Dump ( uint8_t * buffer, int len )
{
  char      ascii_buffer[17];
  char *    ab;
  uint8_t   c;
  int       i, j;
  
  /*  The ascii_buffer is used to accumulate character representations
      for each 16-byte line of data.
  */

  ascii_buffer[16] = 0;   // null-terminate the 16-byte character string

  while ( len > 0 ) {
    ab = ascii_buffer;    // pointer to the current location in the buffer

    if ( len < 16 ) {
      ascii_buffer[len] = 0;    // if the last line of data is less than 16 bytes, we need to terminate accordingly
    }

    for ( i = 0; i < 16; i += 4 ) {   // bytes are output in groups of four
      for ( j = 0; j < 4; j++ ) {      
        if ( i + j < len ) {          // if there is still data to print ...
          c = *buffer;
          buffer++;

          printf("%02x ", c);         // print the byte of data as 2-digit hex

          *ab = ( c < 0x20 ? '.' : ( c > 0x7f ? '.' : c ));     // if the byte is not a printable character, substitute '.'
          ab++;
        } else {
          print("   ");     // if no data left, print blank spaces
        }
      }

      print(" ");     // extra space between each set of four bytes
    }

    println(ascii_buffer);    // print string represenation after the 16 bytes
    len -= 16;
  }
  
  return *this;     // return reference to DUMP object to allow streaming
}

size_t DEBUG::write ( uint8_t byte )
{
  /*  If the current filter setting does not include
      the current output type, discard the byte and
      return 0
  */

  if ( ! ( m_output_type & m_filter ) )
  {
    return 0;

  /*  Otherwise, store the byte in the buffer. If the byte =
      '\n' or the buffer is full, flush it to the current
      output channel.
  */

  }
  else
  {
    buffer[index++] = byte;

    if ( byte == '\n' || index >= DEBUG_BUFFER_SIZE )
    {
      flush();
    }

    /*  indicate the successful "write" of the character  */

    return 1;
  }
}

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