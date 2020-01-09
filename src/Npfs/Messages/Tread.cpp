/*
 * Tread.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tread.h"

namespace Npfs
{

Tread::Tread(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tread, tag), offset(0), count(0)
{
  fidValue = io.getUI32();
  offset = io.getUI64();
  count = io.getUI32();

#ifndef NDEBUG
  logHeader("Tread");
  logFid();
  log("offset", (unsigned) offset);
  log("count", count);
  logNewLine();
#endif

}

//Tread::~Tread()
//{
//  // Auto-generated destructor stub
//}

}
