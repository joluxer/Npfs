/*
 * Tflush.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Tflush.h"

namespace Npfs
{

Tflush::Tflush(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Tflush, tag)
{
  oldTag = io.getUI16();

#ifndef NDEBUG
  logHeader("Tflush");
  log("oldtag", oldTag);
  logNewLine();
#endif
}

//Tflush::~Tflush()
//{
//  // Auto-generated destructor stub
//}

}
