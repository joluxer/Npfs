/*
 * FcWrite.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcWrite.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcWrite::FcWrite(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool, uint32_t _msize)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Twrite*> (tc)), msize(_msize), rMsg(0), writeOffset(0)
{}

FcWrite::~FcWrite()
{
  delete tMsg;
}

PT_THREAD(FcWrite::engine())
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
//      int n;
//      n = conn->srv->auth->write(fid, tc->offset,
//          tc->count, tc->data);
//      if (n >= 0)
//        rc = sp_create_rwrite(n);
//
//      PT_EXIT(pt);
//    }
//    else
    {
      createError(tMsg, ErrorString::Ebadusefid);
      PT_EXIT(pt);
    }
  }

  if ((fid->omode == (uint16_t)~0) || ((fid->omode & 3) == Oread) || fid->dirEntry->isDir())
  {
    createError(tMsg, ErrorString::Ebadusefid);
    PT_EXIT(pt);
  }

  if ((tMsg->count + tMsg->IoHdrSz) > msize)
  {
    createError(tMsg, ErrorString::Etoolarge);
    PT_EXIT(pt);
  }

  rMessage = rMsg = new(mm) Rwrite(*tMsg);

  if (!handleNoMemError(rMsg))
  {
    rMsg->count = tMsg->count;
    writeOffset = tMsg->offset;
    if ((fid->omode & Oappend) || fid->dirEntry->isAppendOnly())
    {
      writeOffset = fid->dirEntry->size();
    }
    PT_WAIT_WHILE(pt, runOp());
  }

  PT_END(pt);

  return 0;
}

bool FcWrite::runOp()
{
  bool running = true;

  const char* result;

  // ressource is checked as dirEntry, so just acces it (walk has set this pointer)
  OpenIoState s(fid->ioRef, op);
  result = fid->dirEntry->write(s, tMsg->data, rMsg->count, writeOffset);
  if (result != 0)
  {
    running = false;
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
    else if (Resource::OpSuccess == result)
    {
      fid->dirEntry->setMtime(getCurrentTime(), *fid->user);
    }
  }

  return running;
}

}
