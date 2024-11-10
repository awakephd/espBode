/*!
  @file   awg_server.cpp
  @brief  Defines the methods of the AWG_Server class
*/

#include "awg_server.h"

AWG_Server::~AWG_Server ()
{
}

uint32_t  AWG_Server::baud_rate ()
{
  return 115200;
}

uint32_t  AWG_Server::channels ()
{
  return 2;
}

const char * AWG_Server::id ()
{
  return "IDN-SGLT-PRI SDG1062X";
}
