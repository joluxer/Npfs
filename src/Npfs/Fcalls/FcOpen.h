/*
 * FcOpen.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCOPEN_H_
#define FCOPEN_H_

#include "FsFcall.h"

#include "../Messages/Topen.h"
#include "../Messages/Ropen.h"

namespace Npfs
{

class FcOpen: public Npfs::FsFcall
{
public:
  FcOpen(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcOpen();

protected:
  const Topen *const tMsg;
  Ropen* rMsg;

  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCOPEN_H_ */
