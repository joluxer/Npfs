/*
 * FcRemove.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCREMOVE_H_
#define FCREMOVE_H_

#include "FsFcall.h"

#include "../Messages/Tremove.h"
#include "../Messages/Rremove.h"

namespace Npfs
{

class FcRemove: public Npfs::FsFcall
{
public:
  FcRemove(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcRemove();

protected:
  const Tremove *const tMsg;
  Rremove* rMsg;
  Resource* toRemoveResRef;

  virtual
  PT_THREAD(engine());
  bool runClunk();
  bool runRemove();

};

}

#endif /* FCREMOVE_H_ */
