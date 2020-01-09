/*
 * Rclunk.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Rclunk.h"

namespace Npfs
{

Rclunk::Rclunk(const Npfs::Tclunk& tref)
: Rmessage(tref)
{}

//Rclunk::~Rclunk()
//{
//  // Auto-generated destructor stub
//}

void Rclunk::serialize()
{
#ifndef NDEBUG
  logHeader("Rclunk");
  logNewLine();
#endif

  serializeHeader();
}

}
