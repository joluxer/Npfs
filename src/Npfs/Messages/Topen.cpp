/*
 * Topen.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Topen.h"

namespace Npfs
{

Topen::Topen(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Topen, tag), mode(0)
{
  fidValue = io.getUI32();
  mode = io.getUI8();

#ifndef NDEBUG
  logHeader("Topen");
  logFid();
  log("mode", mode);
  logNewLine();
#endif
}

//Topen::~Topen()
//{
//  // Auto-generated destructor stub
//}

}
