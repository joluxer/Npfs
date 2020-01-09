/*
 * Tversion.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tversion.h"

#include <string.h>

namespace Npfs
{

Tversion::Tversion(MsgSerDes& io, uint16_t _tag)
: Tmessage(io, Message::Tversion), mSize(0)
{
  tagValue = _tag;

  version.len = 0;
  version.str = 0;

  mSize = io.getUI32();
  io.get(version);

#ifndef NDEBUG
  logHeader("Tversion");
  log("msize", mSize);
  log("version", version);
  logNewLine();
#endif
}

Tversion::~Tversion()
{
  io.release(version);
}

bool Tversion::is9P2000() const
{
  bool is9p2k;

  is9p2k = (version.len >= 6);
  is9p2k &= (memcmp(version.str, "9P2000", 6) == 0);

  return is9p2k;
}

}
