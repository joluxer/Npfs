/*
 * Rwrite.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rwrite.h"

namespace Npfs
{

Rwrite::Rwrite(const Npfs::Twrite& tref)
: Rmessage(tref), count(tref.count)
{}

//Rwrite::~Rwrite()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rwrite::msgLength() const
{
  return 4;  /* count[4] */
}

void Rwrite::serialize()
{
#ifndef NDEBUG
  logHeader("Rwrite");
  log("count", count);
  logNewLine();
#endif

  serializeHeader();
  io.putUI32(count);
}

}
