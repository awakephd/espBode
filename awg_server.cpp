#include "awg_server.h"

/*** destructor ****************************************

  The default destructor does nothing. However, this
  is declared as a virtual destructor so that descendant
  classes can handle any necessary housekeeping.
  
********************************************************/

AWG_Server::~AWG_Server ()
{
}

/*** baud_rate() ****************************************

  Most versions of AWG used with the espBode system
  will communicate via Serial at a baud rate of 115200.
  If a different rate is needed, this method can be
  overridden.
  
********************************************************/

uint32_t  AWG_Server::baud_rate ()
{
  return 115200;
}

/*** channels() ****************************************

  Most versions of AWG used with the espBode system
  will have 2 channels. This method can be overridden
  if needed to indicate a different number of channels.
  
********************************************************/

uint32_t  AWG_Server::channels ()
{
  return 2;
}

/*** id() ***********************************************

  Most versions of AWG used with the espBode system will
  be fine with the default Siglent AWG identifier below.
  However, this method can be overridden if needed.
  
********************************************************/

const char * AWG_Server::id ()
{
  return "IDN-SGLT-PRI SDG1062X";
}
