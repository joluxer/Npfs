/*
 * Message.h
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "../MsgSerDes.h"
#include "../NpfsConstants.h"
#include "../NpfsNew.h"

#include "ByteStream.h"
#include "MemoryManager.h"

#include <stdint.h>

namespace Npfs
{

class Message
{
public:
  virtual ~Message();

  enum MessageType
  {
    Tfirst    = 100,
    Tversion  = Tfirst,
    Rversion,
    Tauth   = 102,
    Rauth,
    Tattach   = 104,
    Rattach,
    Terror    = 106,
    Rerror,
    Tflush    = 108,
    Rflush,
    Twalk   = 110,
    Rwalk,
    Topen   = 112,
    Ropen,
    Tcreate   = 114,
    Rcreate,
    Tread   = 116,
    Rread,
    Twrite    = 118,
    Rwrite,
    Tclunk    = 120,
    Rclunk,
    Tremove   = 122,
    Rremove,
    Tstat   = 124,
    Rstat,
    Twstat    = 126,
    Rwstat,
    Rlast
  };

  static const unsigned HeaderSize = 7; ///< size[4], id[1], tag[2]

  uint16_t tag() const;
  uint32_t fid() const;

  const MessageType type;

  MsgSerDes& io;

  void* operator new(size_t size, MemoryManager* mm) throw();
  void operator delete(void *) throw();

  static ByteStream* logIo;

  static const unsigned IoHdrSz = 24;

protected:
  Message(MsgSerDes& io, MessageType type, uint16_t tag = (uint16_t)(~0));

  uint16_t tagValue;
  uint32_t fidValue;


  // Logging stuff
  void logHeader(const char* msgType) const;
  void logFid() const;
  void log(const char* tag, uint32_t i) const;
  void log(const char* tag, const NpStr& str) const;
  void log(const char* tag, const NpQid& qid) const;
  void log(const char* tag, const NpStat& stat) const;
  void logPerm(const char* tag, uint32_t perm) const;
  void logNewLine() const;
};

inline
uint16_t Message::tag() const
{
  return tagValue;
}

inline
uint32_t Message::fid() const
{
  return fidValue;
}

inline
void * Message::operator new(size_t size, MemoryManager* mm) throw()
{
  return npfsNew(size, mm);
}

inline
void Message::operator delete(void *mem) throw()
{
  npfsDelete(mem);
}

}

#endif /* MESSAGE_H_ */
