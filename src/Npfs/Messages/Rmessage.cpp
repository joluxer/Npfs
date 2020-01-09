/*
 * Rmessage.cpp
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#include "Rmessage.h"

#include "../Connection.h"

namespace Npfs
{

Rmessage::Rmessage(const Tmessage& tref)
: Message(tref.io, (MessageType)(tref.type + 1), tref.tag()), next(0)
{}

Rmessage::Rmessage(const Tmessage& tref, MessageType _type)
: Message(tref.io, _type, tref.tag()), next(0)
{}

Rmessage::Rmessage(MsgSerDes& io, uint16_t _tag, MessageType _type)
: Message(io, _type, _tag), next(0)
{}

//Rmessage::~Rmessage()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rmessage::msgLength() const
{
  return 0;
}

void Rmessage::serializeHeader()
{
  uint32_t size = msgLength();
  size += 4 + 1 + 2; /* size[4] id[1] tag[2] */
  io.putUI32(size);
  io.putUI8(type);
  io.putUI16(tagValue);
}

void Rmessage::append(Rmessage* n)
{
  // suche das Ende der Kette und hänge an
  Rmessage* m = this;

  while(0 != m->next)
  {
    m = m->next;
  }

  m->next = n;
}

Rmessage* Rmessage::removeGetNext()
{
  Rmessage* ret = next;

  next = 0;

  return ret;
}

}
