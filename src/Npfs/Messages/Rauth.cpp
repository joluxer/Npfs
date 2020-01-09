/*
 * Rauth.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rauth.h"

namespace Npfs
{

Rauth::Rauth(const Npfs::Tauth& tref)
: Rmessage(tref)
{
  aqid.path = 0;
  aqid.type = 0;
  aqid.version = 0;
}


//Rauth::~Rauth()
//{
//  // Auto-generated destructor stub
//}

uint32_t Rauth::msgLength() const
{
  return 13; /* qid[13] */
}

void Rauth::serialize()
{
#ifndef NDEBUG
  logHeader("Rauth");
  log("qid", aqid);
  logNewLine();
#endif
  serializeHeader();
  io.put(aqid);
}

}
