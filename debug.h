#ifndef DEBUG_H
#define DEBUG_H

#include "telnet_server.h"
#include "Streaming.h"

// if the following is defined, a DEBUG instance named Debug will be created in debug.cpp

#define INSTANTIATE_DEBUG

const size_t  DEBUG_BUFFER_SIZE = 128;

/**********************************************************
  The DEBUG class inherits from Print so that it can use
  all of its methods and so that it can respond to the
  methods defined by Streaming.h. It buffers the characters
  received in an internal buffer. This is not actually
  necessary for Serial output, but Telnet output does not
  seem to buffer adequately. Using the internal buffer
  improves performance significantly.
**********************************************************/

class DEBUG : public Print
{
  public:

    // DEBUG will direct output either to Serial or to Telnet

    enum db_channel {
      VIA_SERIAL,
      VIA_TELNET
    };

    /*  DEBUG can selectively omit messages depending on the currently-set
        debug_level. The default is ERROR, for which only errors
        are reported. The PROGRESS level will output progress messages
        (e.g., connection established, command sent, etc.) in addition to
        error messages. The DETAIL level will output errors and progress,
        but also will allow exhaustive debugging information such as the
        "dump" of buffers. */
    
    enum db_level {
      NONE,
      ERROR,
      PROGRESS,
      DETAIL
    };

  public:

    DEBUG ( db_channel channel = VIA_SERIAL, db_level level = ERROR )
      : m_channel(channel), m_debug_level(level), m_output_level(ERROR), index(0)
      {}

    /*  The m_channel field, which can be set by Channel() or in the constructor,
        determines whether output will go to Serial or to Telnet. Note that it
        is acceptable to switch back and forth between these options. */
  
    void Channel ( db_channel channel )
      { m_channel = max(min(channel,VIA_TELNET),VIA_SERIAL); }
    
    db_channel Channel ()
      { return m_channel; }

    /*  The m_debug_level field, which can be set by Level() or set in the constructor,
        determines the maximum allowed level of messages that will be output. */

    void Level ( db_level level )
      { m_debug_level = max(min(level,DETAIL),NONE); }

    db_level Level ()
      { return m_debug_level; }

    /*  The m_output_level field, which can be set by Output() or via a stream
        operator (see below), indicates the level of any output that follows.
        Only if the output level <= debug_level will output be sent to Serial
        or to Telnet. This function returns a reference to its object so that
        it can be used in a stream or as the leftmost member of a stream */

    DEBUG & Output ( db_level level )
      { m_output_level = max(min(level,DETAIL),NONE);
        return *this; }

    db_level Output ()
      { return m_output_level; }

    /*  The following member functions are shortcuts for setting the channel
        and debug level */
    
    void Via_Serial ()
      { m_channel = VIA_SERIAL; }

    void Via_Telnet ()
      { m_channel = VIA_TELNET; }
    
    void Level_None ()
      { m_debug_level = NONE; }
    
    void Level_Error ()
      { m_debug_level = ERROR; }

    void Level_Progress ()
      { m_debug_level = PROGRESS; }

    void Level_Detail ()
      { m_debug_level = DETAIL; }

    /*  The following member functions set the output level and return a
        reference to the object; thus these functions can be used either
        as part of a stream or as the leftmost member of a stream operation. */

    DEBUG & Error ()
      { m_output_level = ERROR;
        return *this; }

    DEBUG & Progress ()
      { m_output_level = PROGRESS;
        return *this; }

    DEBUG & Detail ()
      { m_output_level = DETAIL;
        return *this; }

    // The Dump function will output the contents of the buffer as hex / ascii

    DEBUG & Dump ( uint8_t * buffer, int len );

    /*  The write method is the only pure virtual method of Print. It must
        output the provided byte and return 1 if success or 0 if failure.
        If the output_level > the debug level, the byte will be discarded
        and the function will return 0 (failure). If the output level <=
        the debug level, we will store the byte in the buffer until either
        the received byte = '\n' or the buffer is full; at that point we
        call flush to send the buffer out to either Serial or Telnet. */

    virtual size_t  write ( uint8_t byte );

    /*  Flush the buffer to either Serial or Telnet.  */

    virtual void    flush();

  private:

    uint8_t     buffer[DEBUG_BUFFER_SIZE];
    size_t      index;

    db_channel  m_channel;
    db_level    m_debug_level;
    db_level    m_output_level;
};

/********************************************************************
  The following function allows a DEBUG& to be passed to a Print& via
  the stream operator. This allows the use of functions in the DEBUG
  class that set a condition within a stream.

  Examples:
    
    Debug << Debug.Type(DEBUG::ERROR) << "Something bad happened!\n";
    Debug << Debug.Progress() << "We're getting there!\n";
**********************************************************************/

inline Print & operator << ( Print & pstream, DEBUG & debug_arg )
{
  return debug_arg;
}

/*******************************************************
  If we are instantiating an instance of DEBUG (carried
  out in debug.cpp), we need to let others know that
  it exists.
********************************************************/

#ifdef INSTANTIATE_DEBUG
  extern DEBUG Debug;
#endif


#endif