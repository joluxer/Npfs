/*
 * FcStat.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCSTAT_H_
#define FCSTAT_H_

#include "FsFcall.h"

#include "../Messages/Tstat.h"
#include "../Messages/Rstat.h"

namespace Npfs
{

class FcStat: public Npfs::FsFcall
{
public:
  FcStat(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcStat();

protected:
  const Tstat *const tMsg;
  Rstat* rMsg;

  virtual
  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCSTAT_H_ */
