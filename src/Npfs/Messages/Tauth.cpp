/*
 * Tauth.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tauth.h"

namespace Npfs
{

Tauth::Tauth(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tauth)
{
  this->tagValue = tag;

  afid = io.getUI32();
  io.get(uname);
  io.get(aname);

#ifndef NDEBUG
  logHeader("Tauth");
  log("afid", afid);
  log("uname", uname);
  log("aname", aname);
//  if(dotu)
//    log("nuname", n_uname);
  logNewLine();
#endif
}

Tauth::~Tauth()
{
  io.release(uname);
  io.release(aname);
}

}
