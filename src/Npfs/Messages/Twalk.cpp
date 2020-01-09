/*
 * Twalk.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Twalk.h"

#include <assert.h>

namespace Npfs
{

Twalk::Twalk(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Twalk, tag), newfid((typeof(newfid))(~0)), nWnames(0)
{
  fidValue = io.getUI32();
  newfid = io.getUI32();
  nWnames = io.getUI16();

  if ((unsigned)MaxWalkElements <= nWnames)
  {
    deserError = "too many wnames";
  }

  uint32_t i;
  for (i = 0; i < nWnames; i++)
  {
    io.get(wnames[i]);
  }

#ifndef NDEBUG
  logHeader("Twalk");
  logFid();
  log("newfid", newfid);
  log("nwname", nWnames);
  for(i = 0; i < nWnames; i++)
    log("", wnames[i]);
  logNewLine();
#endif
}

Twalk::~Twalk()
{
  uint32_t i;
  for (i = 0; i < nWnames; i++)
  {
    io.release(wnames[i]);
  }
}

}
