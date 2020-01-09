/*
 * Rwalk.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rwalk.h"

namespace Npfs
{

Rwalk::Rwalk(const Npfs::Twalk& tref)
: Rmessage(tref), nwqids(0)
{}

//Rwalk::~Rwalk()
//{
//}

uint32_t Rwalk::msgLength() const
{
  return 2 + nwqids * 13; /* nwqids[2] nwqids*wqid[13] */
}

void Rwalk::serialize()
{
  unsigned i;
#ifndef NDEBUG
  logHeader("Rwalk");
  log("nwqid", nwqids);
  for(i = 0; i < nwqids; i++)
    log("wqid", wqids[i]);
  logNewLine();
#endif

  serializeHeader();
  io.putUI16(nwqids);
  for (i = 0; i < nwqids; i++)
  {
    io.put(wqids[i]);
  }
}

}
