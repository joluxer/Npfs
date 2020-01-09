/*
 * Tcreate.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tcreate.h"

namespace Npfs
{

Tcreate::Tcreate(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tcreate, tag), perm(0), mode(0)
{
  fidValue = io.getUI32();
  io.get(name);
  perm = io.getUI32();
  mode = io.getUI8();

#ifndef NDEBUG
  logHeader("Tcreate");
  logFid();
  log("name", name);
  logPerm("perm", perm);
  log("mode", mode);
//  if (dotu)
//    log("ext", externsion);
  logNewLine();
#endif
}

Tcreate::~Tcreate()
{
  io.release(name);
}

}
