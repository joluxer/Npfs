/*
 * Rerror.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rerror.h"

#include <string.h>

namespace Npfs
{

Rerror::Rerror(const Tmessage& tref, const char* _ename)
: Rmessage(tref, Message::Rerror)
{
  ename = _ename;
}

Rerror::Rerror(MsgSerDes& io, uint16_t tag, const char* _ename)
: Rmessage(io, tag, Message::Rerror)
{
  ename = _ename;
}

Rerror::Rerror(const Tmessage& tref, const NpStr& _ename)
: Rmessage(tref, Message::Rerror), ename(_ename)
{}

//Rerror::~Rerror()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rerror::msgLength() const
{
  return 2 + ename.len;
}

void Rerror::serialize()
{
#ifndef NDEBUG
  logHeader("Rerror");
  log("ename", ename);
  logNewLine();
#endif

  serializeHeader();
  io.put(ename);
}

}
