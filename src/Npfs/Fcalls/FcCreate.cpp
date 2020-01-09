/*
 * FcCreate.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcCreate.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcCreate::FcCreate(MemoryManager* mm, const Tmessage* tc, Fid * * const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Tcreate*> (tc)), rMsg(0)
{}

FcCreate::~FcCreate()
{
  delete tMsg;
}

PT_THREAD(FcCreate::engine())
{
  Resource* r;
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

  if (!fid->dirEntry->isDir())
  {
    createError(tMsg, ErrorString::Enotdir);
    PT_EXIT(pt);
  }

  if ((tMsg->perm & Dmdir) && (tMsg->mode != Oread))
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  // check for existence of the name
  PT_WAIT_WHILE(pt, Directory::WalkIsPending == (r = fid->directory->walkTo(op, tMsg->name)));
  if (0 == r)
  {
    // create entry
    rMessage = rMsg = new(mm) Rcreate(*tMsg);
    if (!handleNoMemError(rMsg))
    {
      if (fid->dirEntry->isDir())
      {
        fid->directory = static_cast<Directory*>(fid->dirEntry.get());
        fid->dirEntry = 0;

        if (!fid->directory->isWritableBy(*fid->user))
        {
          createError(tMsg, ErrorString::Eperm);
          PT_EXIT(pt);
        }

        PT_WAIT_WHILE(pt, runOp());
      }
      else
      {
        createError(tMsg, ErrorString::Eperm);
        PT_EXIT(pt);
      }
    }

  }
  else
  {
    // ressource exists
    createError(tMsg, ErrorString::Eexist);
  }


  PT_END(pt);

  return 0;
}

bool FcCreate::runOp()
{
  bool running = true;

  const char* result;

  OpenIoState s(fid->ioRef, op);

  Resource* newResource = 0;

  result = fid->directory->create(newResource, s, tMsg->name, *fid->user, tMsg->perm, tMsg->mode, rMsg->iounit);
  fid->dirEntry = newResource;

  if (result && (result != Resource::OpSuccess) && !handleNoMemResult(result))
  {
    fid->dirEntry = fid->directory.get();
    fid->directory = fid->dirEntry->getParent();
    createError(tMsg, result);
    running = false;
  }
  else if (result == Resource::OpSuccess)
  {
    fid->dirEntry->setMtime(getCurrentTime(), *fid->user);
    fid->directory->setMtime(getCurrentTime(), *fid->user);
    rMsg->qid = fid->dirEntry->qid();
    fid->omode = tMsg->mode;

    if (fid->dirEntry->isExclusiveOnly() || (tMsg->mode & Oexcl))
    {
      fid->dirEntry->lockFor(fid);
    }

    running = false;
  }

  return running;
}

}
