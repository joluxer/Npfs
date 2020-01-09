/*
 * Rflush.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rflush.h"

namespace Npfs
{

Rflush::Rflush(const Npfs::Tflush& tref)
: Rmessage(tref)
{}

//Rflush::~Rflush()
//{
//  // Auto-generated destructor stub
//}

void Rflush::serialize()
{
#ifndef NDEBUG
  logHeader("Rflush");
  logNewLine();
#endif

  serializeHeader();
}

}
