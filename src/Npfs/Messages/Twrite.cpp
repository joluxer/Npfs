/*
 * Twrite.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Twrite.h"

namespace Npfs
{

Twrite::Twrite(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Twrite, tag), offset(0), count(0), data(0)
{
  fidValue = io.getUI32();
  offset = io.getUI64();
  count = io.getUI32();

  io.get(data, count);

#ifndef NDEBUG
  logHeader("Twrite");
  log("offset", (unsigned) offset);
  log("count", count);
  log("data", NpStr("(not dumped)"));
  logNewLine();
#endif

}

Twrite::~Twrite()
{
  io.release(data);
}

}
