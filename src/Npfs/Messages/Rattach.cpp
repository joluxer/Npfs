/*
 * Rattach.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rattach.h"

namespace Npfs
{

Rattach::Rattach(const Npfs::Tattach& tref)
: Rmessage(tref)
{}

//Rattach::~Rattach()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rattach::msgLength() const
{
  return 13; /* qid[13] */
}

void Rattach::serialize()
{
#ifndef NDEBUG
  logHeader("Rattach");
  log("qid", qid);
  logNewLine();
#endif

  serializeHeader();
  io.put(qid);
}

}
