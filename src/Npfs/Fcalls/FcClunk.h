/*
 * FcClunk.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCCLUNK_H_
#define FCCLUNK_H_

#include "FsFcall.h"

#include "../Messages/Tclunk.h"
#include "../Messages/Rclunk.h"

namespace Npfs
{

class FcClunk: public Npfs::FsFcall
{
public:
  FcClunk(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcClunk();

protected:
  const Tclunk *const tMsg;
  Rclunk* rMsg;
  struct pt clunkState;
  Directory* toClunkDirRef;
  Resource* toRemoveResRef;

  virtual
  PT_THREAD(engine());
  PT_THREAD(runClunk());
  bool runRemove();

};

}

#endif /* FCCLUNK_H_ */
