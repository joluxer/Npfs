/*
 * FcWstat.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcWstat.h"

#include "../NpfsConstants.h"
#include "../Resources/AdmGroup.h"

namespace Npfs
{

FcWstat::FcWstat(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Twstat*> (tc)), rMsg(0)
{}

FcWstat::~FcWstat()
{
  delete tMsg;
}

PT_THREAD(FcWstat::engine())
{
  Resource* r;
  const NpStat* s;
  Resource::ResultMessage err = 0;

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

  // allowed changes are: mode, name, length, mtime, gid

  if ((tMsg->stat.type != (uint16_t)~0) || (tMsg->stat.dev != (uint32_t)~0)
      || (tMsg->stat.qid.version != (uint32_t)~0)
      || (tMsg->stat.qid.path != (uint64_t)~0) )
  {
    createError(tMsg, ErrorString::Eperm);
    PT_EXIT(pt);
  }

  if ((tMsg->stat.mode != (uint32_t)~0) && ((fid->dirEntry->isDir() && !(tMsg->stat.mode & Dmdir))
      || (!fid->dirEntry->isDir() && (tMsg->stat.mode & Dmdir))))
  {
    createError(tMsg, ErrorString::Edirchange);
    PT_EXIT(pt);
  }

  // check permissions for rename
  if (tMsg->stat.name.len != 0)
  {
    if (fid->directory->isWritableBy(*fid->user))
    {
      // check for existence of new name
      PT_WAIT_WHILE(pt, Directory::WalkIsPending == (r = fid->directory->walkTo(op, tMsg->stat.name)));

      if (0 != r)
      {
        createError(tMsg, ErrorString::Eexist);
        PT_EXIT(pt);
      }
    }
    else
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  // check permissions for chmod
  s = &fid->dirEntry->getStat();
  if (tMsg->stat.mode != (uint32_t)~0)
  {
    if ((*fid->user == s->uid) || AdmGroup(s->gid).hasGroupLeader(*fid->user))
    {
      // allow, intentionally do nothing here, but later down
    }
    else
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  // check permissions for changing times
  if (tMsg->stat.mtime != (uint32_t)~0)
  {
    // allow according to the rules, and for linux also in case of truncations...
    if ((tMsg->stat.length != (uint64_t)~0) || (*fid->user == s->uid) || AdmGroup(s->gid).hasGroupLeader(*fid->user))
    {
      // allow, intentionally do nothing here, but later down
    }
    else
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  // check for changing gid
  if (tMsg->stat.gid.len != 0)
  {
    if (((*fid->user == s->uid) || AdmGroup(s->gid).hasGroupLeader(*fid->user)) ||
        (AdmGroup(s->gid).hasGroupLeader(*fid->user) && AdmGroup(tMsg->stat.gid).hasGroupLeader(*fid->user)))
    {
      // allow, intentionally do nothing here, but later down
    }
    else
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  // check truncations
  if (tMsg->stat.length != (uint64_t)~0)
  {
    if (!fid->dirEntry->isWritableBy(*fid->user))
    {
      createError(tMsg, ErrorString::Eperm);
      PT_EXIT(pt);
    }
  }

  if (tMsg->stat.length != (uint64_t)~0)
  {
    PT_WAIT_WHILE(pt, 0 == (err = fid->dirEntry->trunc(op, tMsg->stat.length)));
    if (err != Resource::OpSuccess)
    {
      createError(tMsg, err);
      PT_EXIT(pt);
    }
  }

  rMessage = rMsg = new(mm) Rwstat(*tMsg);
  if (!handleNoMemError(rMsg))
  {
    PT_WAIT_WHILE(pt, runOp());
  }

  PT_END(pt);

  return 0;
}

bool FcWstat::runOp()
{
  bool running = true;

  const char* result = ErrorString::InternalServerError;

  result = fid->dirEntry->modifyStat(op, tMsg->stat);

  if (result != 0)
  {
    running = false;
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
    else if (Resource::OpSuccess == result)
    {
      fid->directory->setMtime(getCurrentTime(), *fid->user);
    }
  }

  return running;
}

}
