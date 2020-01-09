/*
 * Rremove.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rremove.h"

namespace Npfs
{

Rremove::Rremove(const Npfs::Tremove& tref)
: Rmessage(tref)
{}

//Rremove::~Rremove()
//{
//  // Auto-generated destructor stub
//}

void Rremove::serialize()
{
#ifndef NDEBUG
  logHeader("Rremove");
  logNewLine();
#endif

  serializeHeader();
}

}
