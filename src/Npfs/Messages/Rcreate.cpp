/*
 * Rcreate.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rcreate.h"

namespace Npfs
{

Rcreate::Rcreate(const Npfs::Tcreate& tref)
: Rmessage(tref), iounit(0)
{
  qid.path = 0;
  qid.type = 0;
  qid.version = 0;
}

//Rcreate::~Rcreate()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rcreate::msgLength() const
{
  return 13 + 4; /* qid[13] iounit[4] */
}

void Rcreate::serialize()
{
#ifndef NDEBUG
  logHeader("Rcreate");
  log("qid", qid);
  logNewLine();
#endif

  serializeHeader();
  io.put(qid);
  io.putUI32(iounit);
}

}
