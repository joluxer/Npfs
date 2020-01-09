/*
 * FcRemove.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcRemove.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcRemove::FcRemove(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Tremove*> (tc)), rMsg(0),
  toRemoveResRef(0)
{}

FcRemove::~FcRemove()
{
  delete tMsg;
}

PT_THREAD(FcRemove::engine())
{
  struct pt *pt = &state;

  PT_BEGIN(pt);

  fid = Fid::find(fidPool, tMsg->fid());
  if (!fid)
  {
    createError(tMsg, ErrorString::Eunknownfid);
    PT_EXIT(pt);
  }

  // check for parent writable
  if (not fid->dirEntry->getParent()->isWritableBy(*fid->user))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  fid->incRef();
  rMessage = rMsg = new(mm) Rremove(*tMsg);

  if (!handleNoMemError(rMsg))
  {
    PT_WAIT_WHILE(pt, runClunk());

    fid->dirEntry->unlock();

    if (fid->dirEntry->isMounted())
    {
      fid->dirEntry->umountForRemove();
      fid->directory->setMtime(getCurrentTime(), *fid->user);
    }

    toRemoveResRef = fid->dirEntry.release();
    if (not toRemoveResRef->isAccessed())
    {
      PT_WAIT_WHILE(pt, runRemove());
    }

    fid->decRef();
  }

  PT_END(pt);

  return 0;
}

bool FcRemove::runClunk()
{
  bool running = true;

  const char* result = ErrorString::InternalServerError;

  if (fid->dirEntry)
  {
    if(fid->ioRef)
    {
      OpenIoState s(fid->ioRef,op);
      result = fid->dirEntry->close(s);
    }
    else
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

bool FcRemove::runRemove()
{
  bool running = true;

  const char* result = ErrorString::InternalServerError;

  if (toRemoveResRef)
  {
    result = toRemoveResRef->remove(op, *fid->user);
  }

  if (result != 0)
  {
    running = false;
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
    else if (Resource::OpSuccess == result)
    {
      toRemoveResRef = 0;
    }
  }

  return running;
}

}
