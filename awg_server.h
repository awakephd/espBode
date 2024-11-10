#ifndef AWG_SERVER_H
#define AWG_SERVER_H

/*!
  @file   awg_server.h
  @brief  Declares the AWG_Server class.
*/

#include <stdint.h>
#include "scpi.h"

/*!
  @brief  This is the base class for any AWG that will be
          controlled via the espBode program.

  The awg_server must be able to send and receive settings
  to/from the AWG using the list of parameters defined in
  scpi.h. The set() and get() methods in this class are pure
  virtual methods that must be overridden in the descendant
  class. The other virtual methods provide default versions
  which will satisfy most needs but can be overriden if needed.
*/
class AWG_Server
{
  public:

    /*!
      @brief  Constructor optionally sets the retry count.

      @param  retries   Retry count. See the retry() method for additional details.
    */
    AWG_Server ( uint32_t retries = 0 )
      : m_retry_count(retries)
      {}

    /*!
      @brief  Base class destructor does nothing, but it is
              virtual in case a descendant needs to carry out
              some code on destruction.
    */
    virtual ~AWG_Server ();

    /*!
      @brief  Set the retry count.

      If the retry count > 0, the server should attempt to verify
      any value that is set via the the set() method by reading
      back the value using get(). If the value read does not
      match the value set, the server should try again to set the
      value and verify it, up to retry_count times.

      @param  retries   The number of times to retry setting and validating a value.
    */
    void      retry ( uint32_t retries )
      { m_retry_count = retries; }

    /*!
      @brief  Read the current retry count setting.

      @return The current retry count setting.
    */
    uint32_t  retry ()
      { return m_retry_count; }

    /*!
      @brief  Check to see if the server should attempt to validate a set() command.

      Validation is required if the retry count setting > 0.

      @return True if the server should attempt to validate a set() command.
    */
    bool      validate ()
      { return m_retry_count > 0; }

    /*!
      @brief  Provide a valid Siglent AWG id.

      The default id is IDN-SGLT-PRI SDG1062X. This should suffice
      for most implementations; however, the id() member can be
      overridden to provide a different id if needed.

      @return Pointer to a C-style string (null terminated characters)
    */
    virtual const char *  id ();

    /*!
      @brief  Indicate the number of channels the AWG offers.

      At this time, it appears that the Siglent oscilloscope only attempts
      to use a single channel for Bode plots, so it likely does not matter
      how many channels the AWG offers. Nonetheless, this method is included
      for the sake of completeness. The default value provided by the base
      class is 2 channels, but this default can be overridden by a descendant
      as needed.

      @return The number of channels offered by the AWG.
    */
    virtual uint32_t      channels ();

    /*!
      @brief  Indicate the baud rate that should be used to communicate with the AWG.

      Most AWGs seem to operate at 115200 baud, and this is the default
      value provided by the base class. Any device that needs a different
      baud rate should override this method as needed.

      @return The baud rate that should be used to communicate with the AWG.
    */
    virtual uint32_t      baud_rate ();

    /*!
      @brief  Set a specific parameter on the AWG.

      In the base class, this is a pure virtual method; it must be
      overridden in a descendant class to provide the specific method
      needed to set parameters in a given AWG.

      @param  channel   The AWG channel on which to set the parameter.
      @param  parameter The id of the parameter that should be set (see the scpi::parameter_id enumeration).
      @param  value     The value to which the parameter should be set.

      @return True if the value was successfully set.
    */
    virtual bool    set ( uint32_t channel, uint32_t parameter, double value ) = 0;

    /*!
      @brief  Read a specific parameter from the AWG.

      In the base class, this is a pure virtual method; it must be
      overridden in a descendant class to provide the specific method
      needed to read parameters from a given AWG.

      @param  channel   The AWG channel for which to read the parameter.
      @param  parameter The id of the parameter that should be read (see the scpi::parameter_id enumeration).

      @return The value returned by the AWG for the given parameter and channel.
    */
    virtual double  get ( uint32_t channel, uint32_t parameter ) = 0;

  protected:

    uint32_t  m_retry_count;
};

#endif