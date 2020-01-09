/*
 * FcRead.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FcRead.h"

#include "../NpfsConstants.h"

namespace Npfs
{

FcRead::FcRead(MemoryManager* mm, const Tmessage* tc, Fid * * const fidPool, uint32_t _msize)
: FsFcall(mm, tc->tag(), fidPool), tMsg(static_cast<const Tread*> (tc)), rMsg(0), msize(_msize)
{}

FcRead::~FcRead()
{
  delete tMsg;
}

PT_THREAD(FcRead::engine())
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

  if ((tMsg->count + tMsg->IoHdrSz) > msize)
  {
    createError(tMsg, ErrorString::Etoolarge);
    PT_EXIT(pt);
  }

  if (fid->isAuth())
  {
//    int n;
//
//    if (conn->srv->auth)
//    {
//      rc = sp_alloc_rread(tc->count);
//      if (!rc)
//        PT_EXIT(pt);
//
//      n = conn->srv->auth->read(fid, tc->offset, tc->count, rc->data);
//      if (n >= 0)
//        sp_set_rread_count(rc, n);
//      else
//      {
//        free(rc);
//        rc = NULL;
//      }
//    }
//    else
      createError(tMsg, ErrorString::Ebadusefid);

    PT_EXIT(pt);
  }

  if (fid->omode == (uint16_t) ~0 || (fid->omode & 3) == Owrite)
  {
    createError(tMsg, ErrorString::Ebadusefid);
    PT_EXIT(pt);
  }

  rMessage = rMsg = new(mm) Rread(*tMsg, mm);

  if (!handleNoMemError(rMsg))
  {
    PT_WAIT_WHILE(pt, runOp());

    /*
     if (rc && rc->id==Rread && fid->type&Qtdir) {
     fid->diroffset = tc->offset + rc->count;
     }
     */
  }

  PT_END(pt);

  return 0;
}

bool FcRead::runOp()
{
  bool running = true;

  const char* result;

  OpenIoState s(fid->ioRef, op);

  result = fid->dirEntry->read(s, rMsg->data, rMsg->count, tMsg->offset);

  if (result)
  {
    running = false;
    if ((Resource::OpSuccess != result) && !handleNoMemResult(result))
    {
      createError(tMsg, result);
    }
    else if (Resource::OpSuccess == result)
    {
      fid->dirEntry->setAtime(getCurrentTime());
    }
  }

  return running;
}

}
