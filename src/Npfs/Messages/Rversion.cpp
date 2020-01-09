/*
 * Rversion.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rversion.h"

namespace Npfs
{

Rversion::Rversion(const Npfs::Tversion& tref)
: Rmessage(tref), mSize(tref.mSize)
{
  version.str = "9P2000";
  version.len = 6;
}

//Rversion::~Rversion()
//{
//}

uint32_t Rversion::msgLength() const
{
  return 4 + 2 + version.len;
}

void Rversion::serialize()
{
#ifndef NDEBUG
  logHeader("Rversion");
  log("msize", mSize);
  log("version", version);
  logNewLine();
#endif

  serializeHeader();
  io.putUI32(mSize);
  io.put(version);
}

}
