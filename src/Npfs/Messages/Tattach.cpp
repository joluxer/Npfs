/*
 * Tattach.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tattach.h"

namespace Npfs
{

Tattach::Tattach(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tattach, tag), afid((typeof(afid))(~0))
{
  fidValue = io.getUI32();
  afid = io.getUI32();
  io.get(uname);
  io.get(aname);

#ifndef NDEBUG
  logHeader("Tattach");
  logFid();
  log("afid", afid);
  log("uname", uname);
  log("aname", aname);
  logNewLine();
#endif
}

Tattach::~Tattach()
{
  io.release(uname);
  io.release(aname);
}

}
