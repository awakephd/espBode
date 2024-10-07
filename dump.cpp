#include "ESPTelnet.h"
#include <sstream>


String dump ( uint8_t * buffer, int len )
{
  std::stringstream s;
  uint8_t           c;
  int               n1, n2, i, j;

  while ( len ) {
    n1 = len < 16 ? len : 16;

    while ( n1 ) {
      n2 = n1 < 4 ? n1 : 4;

      for ( i = 0; i < n2; i++ ) {
        c = *buffer;
        buffer++;
				s << ( c < 16 ? "0" : "" ) << std::hex << int(c) << " ";
      }

      n1 = n1 - n2;
      s << " ";
    }

    s << std::endl;
    len = len - ( len < 16 ? len : 16 );
  }
  
  String rs = s.str().c_str();
  return rs;
}
