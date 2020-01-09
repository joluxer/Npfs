/*
 * Rread.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rread.h"

namespace Npfs
{

Rread::Rread(const Npfs::Tread& tref, MemoryManager* mm_)
: Rmessage(tref),  mm(mm_), count(tref.count), data((unsigned char*) mm->allocate(tref.count))
{}

Rread::~Rread()
{
  mm->release(data);
}

uint32_t Rread::msgLength() const
{
  return 4 + count; /* count[4] data[count] */
}

void Rread::serialize()
{
#ifndef NDEBUG
  logHeader("Rread");
  log("count", count);
  log("data", NpStr("(not dumped)"));
  logNewLine();
#endif

  serializeHeader();
  io.putUI32(count);
  io.put(data, count);
}

}
