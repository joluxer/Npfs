/*
 * FcWstat.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCWSTAT_H_
#define FCWSTAT_H_

#include "FsFcall.h"

#include "../Messages/Twstat.h"
#include "../Messages/Rwstat.h"

namespace Npfs
{

class FcWstat: public Npfs::FsFcall
{
public:
  FcWstat(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcWstat();

protected:
  const Twstat *const tMsg;
  Rwstat* rMsg;

  virtual
  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCWSTAT_H_ */
