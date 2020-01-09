/*
 * FcStat.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcStat.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcStat::FcStat(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Tstat*> (tc)), rMsg(0)
{}

FcStat::~FcStat()
{
  delete tMsg;
}

PT_THREAD(FcStat::engine())
{
  struct pt *pt = &state;

  PT_BEGIN(pt);

  fid = Fid::find(fidPool, tMsg->fid());
  if (!fid)
  {
    createError(tMsg, ErrorString::Eunknownfid);
    PT_EXIT(pt);
  }
  else
  {
    fid->incRef();
  }

  rMessage = rMsg = new(mm) Rstat(*tMsg);

  if (!handleNoMemError(rMsg))
  {
    PT_WAIT_WHILE(pt, runOp());
  }

  PT_END(pt);

  return 0;
}

bool FcStat::runOp()
{
  bool running = true;

  const char* result = ErrorString::InternalServerError;

  if (fid->dirEntry)
  {
    rMsg->stat = fid->dirEntry->getStat();
    result = Resource::OpSuccess;
  }

  if (result != 0)
  {
    running = false;
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
  }

  return running;
}

}
