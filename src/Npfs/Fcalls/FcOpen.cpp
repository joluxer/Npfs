/*
 * FcOpen.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcOpen.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcOpen::FcOpen(MemoryManager* mm, const Tmessage* tc, Fid * * const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Topen*> (tc)),
  rMsg(0)
{}

FcOpen::~FcOpen()
{
  delete tMsg;
}

PT_THREAD(FcOpen::engine())
{
  const char* result;
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

  if (fid->omode != (uint16_t)~0)
  {
    createError(tMsg, ErrorString::Ebadusefid);
    PT_EXIT(pt);
  }

  if (fid->dirEntry->isLocked())
  {
    createError(tMsg, ErrorString::Eopen);
    PT_EXIT(pt);
  }

  if (fid->dirEntry->isExclusiveOnly() || (tMsg->mode & Oexcl))
  {
    if (!fid->dirEntry->lockFor(fid))
    {
      createError(tMsg, ErrorString::Eopen);
      PT_EXIT(pt);
    }
  }

  if ((((tMsg->mode & 3) == Oread) || ((tMsg->mode & 3) == Ordwr)) && !fid->dirEntry->isReadableBy(*fid->user))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if ((((tMsg->mode & 3) == Owrite) || ((tMsg->mode & 3) == Ordwr)) && !fid->dirEntry->isWritableBy(*fid->user))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if (((tMsg->mode & 3) == Oexec) && !fid->dirEntry->isExecutableBy(*fid->user))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if (tMsg->mode & Otrunc)
  {
    if (fid->dirEntry->isWritableBy(*fid->user))
    {
      if (!fid->dirEntry->isAppendOnly())
      {
        PT_WAIT_WHILE(pt, 0 == (result = fid->dirEntry->trunc(op, 0)));

        if (result && (Resource::OpSuccess != result) && !handleNoMemResult(result))
        {
          createError(tMsg, result);
          PT_EXIT(pt);
        }
        else if (result == Resource::OpSuccess)
        {
          fid->dirEntry->setMtime(getCurrentTime(), *fid->user);
        }
      } // else do not fail, but do not truncate
    }
    else
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  if ((tMsg->mode & Orclose) && !fid->directory->isWritableBy(*fid->user))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if ((fid->dirEntry->isDir()) && (tMsg->mode != Oread))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if (!rMessage)
  {
    rMessage = rMsg = new(mm) Ropen(*tMsg);

    if (!handleNoMemError(rMsg))
    {
      PT_WAIT_WHILE(pt, runOp());
    }
  }

  PT_END(pt);

  return 0;
}

bool FcOpen::runOp()
{
  bool running = true;

  const char* result;

  OpenIoState s(fid->ioRef, op);

  result = fid->dirEntry->open(s, tMsg->mode, rMsg->iounit);

  running = (0 == result);

  if (result && (Resource::OpSuccess != result) && !handleNoMemResult(result))
  {
    createError(tMsg, result);
  }
  else if (result == Resource::OpSuccess)
  {
    rMsg->qid = fid->dirEntry->qid();
    fid->omode = tMsg->mode | (fid->dirEntry->isExclusiveOnly() ? Oexcl : 0);
  }

  return running;
}

}
