/*
 * FcWalk.h
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#ifndef FCWALK_H_
#define FCWALK_H_

#include "FsFcall.h"

#include "../Messages/Twalk.h"
#include "../NpfsConstants.h"

namespace Npfs
{

class FcWalk: public Npfs::FsFcall
{
public:
  FcWalk(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcWalk();

protected:
  const Twalk *const tMsg;
  Fid *newfid;

  typedef const char* ErrStr;

  unsigned idx;
  NpQid wqids[MaxWalkElements];
  struct pt walkStepState;

  PT_THREAD(engine());
  PT_THREAD(doWalkStep(ErrStr& result, Fid* fid, const NpStr& wname, NpQid& wqid));

};

}

#endif /* FCWALK_H_ */
