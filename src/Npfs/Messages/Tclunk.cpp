/*
 * Tclunk.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tclunk.h"

namespace Npfs
{

Tclunk::Tclunk(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tclunk, tag)
{
  fidValue = io.getUI32();

#ifndef NDEBUG
  logHeader("Tclunk");
  logFid();
  logNewLine();
#endif

}

//Tclunk::~Tclunk()
//{
//  // Auto-generated destructor stub
//}

}
