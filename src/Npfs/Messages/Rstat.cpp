/*
 * Rstat.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rstat.h"

namespace Npfs
{

Rstat::Rstat(const Npfs::Tstat& tref)
: Rmessage(tref)
{
  // TODO: stat mit sicheren Werten vorbelegen
}

//Rstat::~Rstat()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rstat::msgLength() const
{
  unsigned statsz = stat.statSize();
  return 2 + 2 + statsz;
}

void Rstat::serialize()
{
#ifndef NDEBUG
  logHeader("Rstat");
  log("stat", stat);
  logNewLine();
#endif

  serializeHeader();
  io.putUI16(stat.statSize() + 2);
  io.put(stat);
}

}
