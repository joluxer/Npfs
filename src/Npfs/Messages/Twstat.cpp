/*
 * Twstat.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Twstat.h"

namespace Npfs
{

Twstat::Twstat(MsgSerDes& io, uint16_t tag)
: Tmessage(io, Message::Twstat, tag)
{
  fidValue = io.getUI32();
  io.getUI16(); // dummy read von zwei bytes, deren Wert nicht verwendet wird? TODO: Fehler-Test?

  // jetzt den stat auslesen
  stat.size = io.getUI16();
  stat.type = io.getUI16();
  stat.dev = io.getUI32();
  io.get(stat.qid);
  stat.mode = io.getUI32();
  stat.atime = io.getUI32();
  stat.mtime = io.getUI32();
  stat.length = io.getUI64();
  io.get(stat.name);
  io.get(stat.uid);
  io.get(stat.gid);
  io.get(stat.muid);

  // jetzt einige Plausibilitaets-Tests
//  uint8_t statType = stat.mode >> 24;
  if (stat.statSize() != stat.size)
    deserError = "message size problem: sent stat.size does not match actual stat size";

#ifndef NDEBUG
  logHeader("Twstat");
  log("wstat", stat);
  logNewLine();
#endif

}

//Twstat::~Twstat()
//{
//  // Auto-generated destructor stub
//}

}
