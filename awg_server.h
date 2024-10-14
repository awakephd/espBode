#ifndef AWG_SERVER_H
#define AWG_SERVER_H

#include <stdint.h>
#include "scpi.h"

/*** class AWG_SERVER **************************************

  This is the base class for any AWG that will be served
  by the ESP-01. The awg_server must be able to send and
  receive settings to/from the AWG using the list of
  parameters defined in scpi.h. The set and get methods in
  this class are pure virtual methods that must be over-
  ridden in the descendant class. The other virtual methods
  provide default versions which will satisfy most needs
  but can be overriden if necessary.

************************************************************/

class AWG_Server
{
  public:

    AWG_Server ( uint32_t retries = 0 )
      : retry_count(retries)
      {}

    virtual ~AWG_Server ();

    void      retry ( uint32_t retries )
      { retry_count = retries; }

    uint32_t  retry ()
      { return retry_count; }

    bool      validate ()
      { return retry_count > 0; }

    virtual const char *  id ();
    virtual uint32_t      channels ();

    virtual uint32_t      baud_rate ();

    virtual bool          set ( uint32_t channel, uint32_t parameter, double value ) = 0;
    virtual double        get ( uint32_t channel, uint32_t parameter ) = 0;

  protected:

    uint32_t  retry_count;
};

#endif