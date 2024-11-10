#ifndef DEBUG_H
#define DEBUG_H

/*!
  @file   debug.h
  @brief  Declares the DEBUG class and associated functions.
*/

#include "Streaming.h"
#include "telnet_server.h"

/*!
  @brief If INSTANTIATE_DEBUG is defined, a DEBUG instance named Debug will be created in debug.cpp
*/
#define INSTANTIATE_DEBUG

/*!
  @brief  Size of buffer used by DEBUG instance.
*/
const size_t  DEBUG_BUFFER_SIZE = 128;

/*!
  @brief  Filters and directs debugging messages either to
          Serial or Telnet.
        
  The DEBUG class inherits from Print so that it can use all of its
  methods and so that it can respond to the methods defined in
  Streaming.h. DEBUG buffers the characters received in an internal
  buffer; this is not actually necessary for Serial output, but
  Telnet output does not seem to buffer adequately. Providing this
  buffer improves Telnet performance significantly.
*/
class DEBUG : public Print
{
  public:

    /*!
      @brief  Where to direct output (Serial or Telnet)
    */
    enum db_channel {
      VIA_SERIAL,     ///< Send debugging output to the Serial port
      VIA_TELNET      ///< Send debugging output through the Telnet server
    };

    /*!
      @brief  Which types of debugging information to output.

      DEBUG filters which message to output depending on the current value
      of the m_filter member. Filters can be combined as desired.
    */
    enum db_filter {
      NONE      = 0,    ///< Suppress all debugging output
      ERROR     = 1,    ///< Include messages marked as ERROR type
      PROGRESS  = 2,    ///< Include messages marked as PROGRESS type, e.g., connection established
      SERIAL_IO = 4,    ///< Include messages marked as SERIAL_IO, e.g., data sent to / received from the Serial port
      PACKET    = 8,    ///< Include messages marked as PACKET type, e.g., data sent to / received from WiFi
      ALL       = 15    ///< Combine all of the above
    };

  public:

    /*!
      @brief  Constructor can optionally set the channel and filter.

      @param  channel VIA_SERIAL or VIA_TELNET
      @param  filter  NONE, ERROR, PROGRESS, etc. (or any combination)
    */
    DEBUG ( db_channel channel = VIA_SERIAL, db_filter filter = ERROR )
      : m_channel(channel), m_filter(filter), m_output_type(NONE), index(0)
      {}

    /*!
      @brief  Set whether output is directed to Serial or Telnet.
      
      Note that it is acceptable to switch back and forth between
      Serial and Telnet output at any time.

      @param  channel VIA_SERIAL or VIA_TELNET  
    */
    void Channel ( db_channel channel )
      { m_channel = max(min(channel,VIA_TELNET),VIA_SERIAL); }
    
    /*!
      @brief  Return the current output channel.

      @return VIA_SERIAL or VIA_TELNET
    */
    db_channel Channel ()
      { return m_channel; }

    /*!
      @brief  Determine which debugging messages are output.

      Filter determines which message types DEBUG will output.
      Other methods are used to indicate the message type for
      any given message.

      @param  filter  NONE, ERROR, PROGRESS, etc. (or any combination)
    */
    void Filter ( db_filter filter )
      { m_filter = max(min(filter,ALL),NONE); }

    /*!
      @brief  Return the current filter setting.

    */
    db_filter Filter ()
      { return m_filter; }

    /*!
      @brief  Set the type of output for any DEBUG messages that follow.

      The m_output_type can be set by Output() or via a stream operator.
      It indicates the type of any following DEBUG messages until a new
      m_output_level is set. Only if (output type & filter) != 0
      will output be sent to Serial or to Telnet. Typically the output
      type will be set to only one of the filter options, but nothing
      prevents setting the output to a combination of types.
      
      @param  type  NONE, ERROR, PROGRESS, etc.

      @return A reference to the DEBUG object (thus allowing use in a stream or as the leftmost member of a stream)
    */
    DEBUG & Output ( db_filter type )
      { m_output_type = max(min(type,ALL),NONE);
        return *this; }

    /*!
      @brief  Return the current output type.

      @return NONE, ERROR, PROGRESS, etc.
    */
    db_filter Output ()
      { return m_output_type; }

    /*!
      @brief  Shortcut method for setting output channel to Serial.
    */
    void Via_Serial ()
      { m_channel = VIA_SERIAL; }

    /*!
      @brief  Shortcut method for setting output channel to Telnet.
    */
    void Via_Telnet ()
      { m_channel = VIA_TELNET; }
    
    /*!
      @brief  Shortcut method for shutting off all output.
    */
    void Filter_None ()
      { m_filter = NONE; }
    
    /*!
      @brief  Shortcut method for outputting only ERROR type messages.
    */
    void Filter_Error ()
      { m_filter = ERROR; }

    /*!
      @brief  Shortcut method for outputting ERROR or PROGRESS type messages.
    */
    void Filter_Progress ()
      { m_filter = (db_filter)(PROGRESS | ERROR); }

    /*!
      @brief  Shortcut method for outputting ERROR, PROGRESS, or SERIAL_IO type messages.
    */
    void Filter_Serial_IO ()
      { m_filter = (db_filter)(PROGRESS | SERIAL_IO | ERROR); }

    /*!
      @brief  Shortcut method for outputting all types of messages.
    */
    void Filter_All ()
      { m_filter = ALL; }

    /*!
      @brief  Set output type as ERROR.
      
      Because this method returns a reference to the DEBUG object,
      it can be used either as part of a stream or as the leftmost
      member of a stream operation.

      @return Reference to the DEBUG object
    */
    DEBUG & Error ()
      { m_output_type = ERROR;
        return *this; }

    /*!
      @brief  Set output type as PROGRESS.
      
      Because this method returns a reference to the DEBUG object,
      it can be used either as part of a stream or as the leftmost
      member of a stream operation.

      @return Reference to the DEBUG object
    */
    DEBUG & Progress ()
      { m_output_type = PROGRESS;
        return *this; }

    /*!
      @brief  Set output type as SERIAL_IO.
      
      Because this method returns a reference to the DEBUG object,
      it can be used either as part of a stream or as the leftmost
      member of a stream operation.

      @return Reference to the DEBUG object
    */
    DEBUG & Serial_IO ()
      { m_output_type = SERIAL_IO;
        return *this; }

    /*!
      @brief  Set output type as PACKET.
      
      Because this method returns a reference to the DEBUG object,
      it can be used either as part of a stream or as the leftmost
      member of a stream operation.

      @return Reference to the DEBUG object
    */
    DEBUG & Packet ()
      { m_output_type = PACKET;
        return *this; }

    /*!
      @brief  Output the contents of a buffer as hex and ASCII.

      The contents of the buffer are output on the current channel
      (Serial or Telnet) 16 bytes per line, with the left side
      formatted as four sets of four bytes in hex, and the right
      side as a string of ASCII characters.

      @param  buffer  Pointer to the data to be dumped
      @param  len     Length of the data to be dumped

      @return Return a reference to the DUMP object to allow streaming
    */
    DEBUG & Dump ( uint8_t * buffer, int len );

    /*!
      @brief  Override the write() method of the Print class.

      The write method is the only pure virtual method of Print. It must
      output the provided byte and return 1 if success or 0 if failure.
      If the current output type does not have a bit in common with the
      current filter setting, the byte will be discarded and the method
      will return 0 (failure). If the current output type does have a
      bit in common with the current filter setting, the method stores
      the byte in the buffer until either the received byte = '\\n' or
      the buffer is full; at that point the buffer is flushed out to
      either Serial or Telnet according to the current channel setting.

      @param  byte  The current byte of data to filter or buffer.

      @return 0 if the byte is discarded; 1 if it is output
    */
    virtual size_t  write ( uint8_t byte );

    /*!
      @brief  Send the contents of the buffer to Serial or Telnet.

      This is an override of the virtual flush() function from the
      Print class. If there is any data in the buffer, it is sent
      to Serial or to Telnet (according to m_channel) and the buffer
      index is reset.
    */
    virtual void    flush();

  private:

    uint8_t     buffer[DEBUG_BUFFER_SIZE];    ///< Buffer to improve performance
    size_t      index;                        ///< Position for next entry into the buffer

    db_channel  m_channel;                    ///< The current output channel (VIA_SERIAL or VIA_TELNET)
    db_filter   m_filter;                     ///< The current filter to determine which messages are output
    db_filter   m_output_type;                ///< The current type to consider any output received
};

/*!
  @brief  Helper function to facilitate stream operations on DEBUG objects.

  This function overrides the << operator to allow a DEBUG& object
  to be streamed to a Print& object. This allows the use of DEBUG
  methods that set a condition within the stream.

  Examples:
    
    Debug << Debug.Type(DEBUG::ERROR) << "Something bad happened!\n"\n

    Debug << Debug.Progress() << "We're getting there!\n";

  @param  pstream   Reference to a Print object (destination of the stream operation)
  @param  debug_arg A method that returns a reference to the DEBUG object

  @return The debug_arg is passed through as the return object.
*/
inline DEBUG & operator << ( Print & pstream, DEBUG & debug_arg )
{
  return debug_arg;
}

/*  If we are instantiating an instance of DEBUG (carried
    out in debug.cpp), we need to let others know that
    it exists.
*/

#ifdef INSTANTIATE_DEBUG
  extern DEBUG Debug;
#endif

#endif