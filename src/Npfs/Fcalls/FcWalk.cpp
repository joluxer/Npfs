/*
 * FcWalk.cpp
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#include "FcWalk.h"

#include "../Messages/Rwalk.h"

namespace Npfs
{

FcWalk::FcWalk(MemoryManager* mm, const Tmessage* tc, Fid * * const fidPool) :
  FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Twalk*> (tc)), newfid(0)
{
  PT_INIT(&walkStepState);
}

FcWalk::~FcWalk()
{
  delete tMsg;
}

PT_THREAD(FcWalk::engine())
{
  unsigned n;
  struct pt *pt = &state;

  ErrStr err = 0;

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

  if (tMsg->nWnames && !fid->dirEntry->isDir())
  {
    createError(tMsg, ErrorString::Enotdir);
    PT_EXIT(pt);
  }

  if (fid->omode != (typeof(fid->omode)) ~0)
  {
    createError(tMsg, ErrorString::Ebadusefid);
    PT_EXIT(pt);
  }

  if (tMsg->nWnames > (unsigned)MaxWalkElements)
  {
    createError(tMsg, ErrorString::Etoomanywnames);
    PT_EXIT(pt);
  }

  if (tMsg->fid() != tMsg->newfid)
  {
    newfid = Fid::find(fidPool, tMsg->newfid);
    if (newfid)
    {
      createError(tMsg, ErrorString::Einuse);
      PT_EXIT(pt);
    }
    newfid = Fid::create(fidPool, tMsg->newfid, mm);
    if (handleNoMemError(newfid))
    {
      PT_EXIT(pt);
    }

    newfid->clone(fid);

  }
  else
  {
    newfid = fid;
  }

  newfid->incRef();
  for(idx = 0; idx < tMsg->nWnames;)
  {
    PT_SPAWN(pt, &walkStepState, doWalkStep(err, newfid, tMsg->wnames[idx], wqids[idx]));
    if (err)
    {
      createError(tMsg, err);
      newfid->decRef();
      PT_EXIT(pt);
    }

    idx++;

    if (idx<(tMsg->nWnames) && !newfid->dirEntry->isDir())
    {
      createError(tMsg, ErrorString::Eperm);
      newfid->decRef();
      PT_EXIT(pt);
    }
  }

  if ((idx == 0) && (tMsg->nWnames != 0)) // TODO: warum das jetzt??? Das stammt aus dem Original-Code und ich hab in den Specs noch keine Grund dafür gefunden, evtl. ist es ja logisch und ich überschau das gerade nicht.
  {
    PT_EXIT(pt);
  }

  if (!rMessage) // no error yet
  {
    if (fid->fid != newfid->fid)
    {
      newfid->incRef();
    }
    Rwalk* reply;
    reply = new(mm) Rwalk(*tMsg);
    rMessage = reply;
    if (!handleNoMemError(reply))
    {
      reply->nwqids = idx;
      if (!handleNoMemError(reply->wqids))
      {
        for(n = 0; n < idx; n++)
        {
          reply->wqids[n] = this->wqids[n];
        }
      }
      else
      {
        newfid->decRef();
      }
    }
  }

  newfid->decRef();

  PT_END(pt);

  return 0;
}

PT_THREAD(FcWalk::doWalkStep(ErrStr& result, Fid* f, const NpStr& wname, NpQid& wqid))
{
  Resource* resource;
  static const NpStr dotdot("..", 2);
  struct pt *pt = &walkStepState;

  result = 0;

  PT_BEGIN(pt);

  if (0 != wname.len) // ein wname gibt das Ziel an, ein leerer wname wird wie "." behandelt und ist immer erfolgreich
  {
    if (dotdot == wname)
    {
      fid->dirEntry = f->directory.get();
      f->directory = fid->dirEntry->getParent();
    }
    else if (f->dirEntry->isExecutableBy(*f->user))
    {
      PT_WAIT_WHILE(pt, Directory::WalkIsPending == (resource = f->dirEntry->walkTo(op, wname)));
      if (0 != resource)
      {
        f->directory = static_cast<Directory*>(f->dirEntry.get());
        f->dirEntry = resource;
        wqid = resource->qid();
      }
      else
      {
        result = ErrorString::Enotfound;
      }
    }
    else
    {
      result = ErrorString::Eperm;
    }
  }

  PT_END(pt);

  return 0;
}

}
