/*
 * Ropen.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Ropen.h"

namespace Npfs
{

Ropen::Ropen(const Npfs::Topen& tref)
: Rmessage(tref), iounit(0)
{
  qid.path = 0;
  qid.type = 0;
  qid.version = 0;
}

//Ropen::~Ropen()
//{
//  // Auto-generated destructor stub
//}

uint32_t Ropen::msgLength() const
{
  return 13 + 4; /* qid[13] iounit[4] */
}

void Ropen::serialize()
{
#ifndef NDEBUG
  logHeader("Ropen");
  log("qid", qid);
  log("iounit", iounit);
  logNewLine();
#endif

  serializeHeader();
  io.put(qid);
  io.putUI32(iounit);
}

}
