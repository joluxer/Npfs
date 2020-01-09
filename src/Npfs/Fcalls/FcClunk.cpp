/*
 * FcClunk.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcClunk.h"
#include "../NpfsConstants.h"

#include "pt/pt.h"

namespace Npfs
{

FcClunk::FcClunk(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Tclunk*> (tc)), rMsg(0),
  toClunkDirRef(0), toRemoveResRef(0)
{}

FcClunk::~FcClunk()
{
  delete tMsg;
}

PT_THREAD(FcClunk::engine())
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

  if (fid->isAuth())
  {
//    if (conn->srv->auth)
//    {
//      n = conn->srv->auth->clunk(fid);
//      if (n)
//        rc = sp_create_rclunk();
//    }
//    else
    {
      createError(tMsg, ErrorString::Ebadusefid);
      PT_EXIT(pt);
    }
  }

  rMessage = rMsg = new(mm) Rclunk(*tMsg);
  if (!handleNoMemError(rMsg))
  {
    PT_SPAWN(pt, &clunkState, runClunk());

    if (toRemoveResRef && not toRemoveResRef->isMounted() && not toRemoveResRef->isAccessed())
    {
      PT_WAIT_WHILE(pt, runRemove());
    }

    fid->decRef();
  }

  PT_END(pt);

  return 0;
}

PT_THREAD(FcClunk::runClunk())
{
  struct pt*const pt = &clunkState;
  const char* result = ErrorString::InternalServerError;

  PT_BEGIN(pt);

  if (fid->dirEntry && fid->ioRef)
  {
    OpenIoState s(fid->ioRef, op);
    result = fid->dirEntry->close(s);
  }
  else
  {
    result = Resource::OpSuccess;
  }

  if (result != 0)
  {
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
    else
    {
      if (fid->nsDetach)
      {
        fid->dirEntry.release();
        toClunkDirRef = fid->directory.release();
        PT_WAIT_WHILE(pt, fid->nsDetach->execute(toClunkDirRef));
        PT_EXIT(pt);
      }
      else if (fid->omode != (uint16_t)~0)
      {
        // file was successful opened
        if (fid->omode & Oexcl)
        {
          // file was locked
          fid->dirEntry->unlock();
        }
        if (fid->omode & Orclose)
        {
          if (fid->dirEntry->isMounted())
          {
            fid->dirEntry->umountForRemove();
            fid->directory->setMtime(getCurrentTime(), *fid->user);
          }
        }
      }

      toRemoveResRef = fid->dirEntry.release();
    }
  }

  PT_END(pt);

  return 0;
}

bool FcClunk::runRemove()
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

    if (ErrorString::InternalServerError != result)
      result = Resource::OpSuccess;

    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
      createError(tMsg, result);
    else if (Resource::OpSuccess == result)
      toRemoveResRef = 0;
  }

  return running;
}

}
