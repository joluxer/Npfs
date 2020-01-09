/*
 * Tmessage.cpp
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#include "Tmessage.h"

#include "../Connection.h"
#include "../MsgBufferSerDes.h"
#include "Tversion.h"
#include "Tauth.h"
#include "Tflush.h"
#include "Tattach.h"
#include "Twalk.h"
#include "Topen.h"
#include "Tcreate.h"
#include "Tread.h"
#include "Twrite.h"
#include "Tclunk.h"
#include "Tremove.h"
#include "Tstat.h"
#include "Twstat.h"

#include "Rerror.h"

#include <assert.h>

namespace Npfs
{

Tmessage::Tmessage(MsgSerDes& io, MessageType type, uint16_t _tag)
: Message(io, type, _tag), next(0)
{}

//Tmessage::~Tmessage()
//{
//  // Auto-generated destructor stub
//}

bool Tmessage::isBufferComplete(unsigned char* bufferHeader, unsigned rxLength)
{
  MsgBufferSerDes header(bufferHeader, 0, HeaderSize);
  uint32_t msgLength = header.getUI32();

  return (msgLength <= rxLength);
}

Tmessage* Tmessage::create(MemoryManager* mm, unsigned char* bufferHeader, MsgSerDes& io, unsigned maxMsgSize)
{
  Tmessage* msg = 0;
  const char* err = 0;

  MsgBufferSerDes header(bufferHeader, 0, HeaderSize);
  uint32_t msgLength = header.getUI32();

  if (msgLength > maxMsgSize)
  {
    msgLength = maxMsgSize;
    err = "data error: message length exceeds max. message size";
  }
  else
  {
    MessageType mType = (MessageType)header.getUI8();
    uint16_t mTag = header.getUI16();

    unsigned consumedBytes = header.consumedByteCount();

    switch (mType)
    {
    default:
      break;

    case Tversion:
      msg = new(mm) Npfs::Tversion(io, mTag);
      break;

    case Tauth:
      msg = new(mm) Npfs::Tauth(io, mTag);
      break;

    case Tflush:
      msg = new(mm) Npfs::Tflush(io, mTag);
      break;

    case Tattach:
      msg = new(mm) Npfs::Tattach(io, mTag);
      break;

    case Twalk:
      msg = new(mm) Npfs::Twalk(io, mTag);
      break;

    case Topen:
      msg = new(mm) Npfs::Topen(io, mTag);
      break;

    case Tcreate:
      msg = new(mm) Npfs::Tcreate(io, mTag);
      break;

    case Tread:
      msg = new(mm) Npfs::Tread(io, mTag);
      break;

    case Twrite:
      msg = new(mm) Npfs::Twrite(io, mTag);
      break;

    case Tclunk:
      msg = new(mm) Npfs::Tclunk(io, mTag);
      break;

    case Tremove:
      msg = new(mm) Npfs::Tremove(io, mTag);
      break;

    case Tstat:
      msg = new(mm) Npfs::Tstat(io, mTag);
      break;

    case Twstat:
      msg = new(mm) Npfs::Twstat(io, mTag);
      break;
    }

    if (0 == msg)
    {
      err = "unknown message type";
    }
    else
    {
      consumedBytes = io.consumedByteCount();

      if (msgLength > consumedBytes)
      {
        err = "serious error: message size was larger than consumed byte count";
      }
      else if (msgLength < consumedBytes)
      {
        err = "serious error: message size was smaller than consumed byte count";
      }
    }
  }

  if (err)
  {
    msg->deserError = err;
  }

  return msg;
}

const NpStr* Tmessage::getError() const
{
  const NpStr* ret = 0;

  if (deserError.len)
  {
    ret = &deserError;
  }

  return ret;
}

void Tmessage::append(Tmessage* n)
{
  // suche das Ende der Kette und hänge an
  Tmessage* m = next;

  while(0 != m->next)
  {
    m = m->next;
  }

  m->next = n;
}

Tmessage* Tmessage::removeNext()
{
  Tmessage* ret = next;

  next = 0;

  return ret;
}

}
