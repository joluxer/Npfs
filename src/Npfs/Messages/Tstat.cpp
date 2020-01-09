/*
 * Tstat.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tstat.h"

namespace Npfs
{

Tstat::Tstat(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tstat, tag)
{
  fidValue = io.getUI32();

#ifndef NDEBUG
  logHeader("Tstat");
  logFid();
  logNewLine();
#endif

}

//Tstat::~Tstat()
//{
//  // Auto-generated destructor stub
//}

}
