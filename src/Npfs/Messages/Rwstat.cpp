/*
 * Rwstat.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rwstat.h"

namespace Npfs
{

Rwstat::Rwstat(const Npfs::Twstat& tref)
: Rmessage(tref)
{}

//Rwstat::~Rwstat()
//{
//  // Auto-generated destructor stub
//}

void Rwstat::serialize()
{
#ifndef NDEBUG
  logHeader("Rwstat");
  logNewLine();
#endif

  serializeHeader();
}

}
