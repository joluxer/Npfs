/*
 * Tremove.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tremove.h"

namespace Npfs
{

Tremove::Tremove(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tremove, tag)
{
  fidValue = io.getUI32();

#ifndef NDEBUG
  logHeader("Tremove");
  logFid();
  logNewLine();
#endif

}

//Tremove::~Tremove()
//{
//  // Auto-generated destructor stub
//}

}
