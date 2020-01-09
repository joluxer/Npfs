/*
 * FcVersion.cpp
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#include "FcVersion.h"
#include "../Messages/Rversion.h"

namespace Npfs
{

FcVersion::FcVersion(MemoryManager* mm, const Tmessage* tc, Fcall* &wq, Fid** &_fidPool, Connection& _conn)
: Fcall(mm, tc->tag()), tmsg(static_cast<const Tversion*>(tc)), rMsg(0), workQueue(wq), fidPool(_fidPool), conn(_conn),
  closeState(0)
{
  PT_INIT(&fcState);
}

FcVersion::~FcVersion()
{
  delete tmsg;
}

Rmessage* FcVersion::execute()
{
  Rmessage* reply;

  if (PT_SCHEDULE(engine()))
  {
    // fcall not complete
    reply = 0;
  }
  else
  {
    // fcall complete, result available
    reply = rMessage;
    rMessage = 0;
  }

  return reply;
}

PT_THREAD(FcVersion::engine())
{
  struct pt *pt = &fcState;

  PT_BEGIN(pt);

  if (tmsg->mSize < (tmsg->IoHdrSz + 1))
  {
    createError(tmsg, "msize too small");
    PT_EXIT(pt);
  }

  if (!tmsg->is9P2000())
  {
    createError(tmsg, "unsupported 9P version");
    PT_EXIT(pt);
  }

  rMessage = rMsg = new(mm) Rversion(*tmsg);

  if (!handleNoMemError(rMsg))
  {
    rMsg->mSize = tmsg->mSize < conn.maxMsgSize() ? tmsg->mSize : conn.maxMsgSize();
    conn.setMsize(rMsg->mSize);
  }

  PT_SPAWN(pt, &connResetState, connectionReset());

  PT_END(pt);

  return 0;
}

inline
bool FcVersion::doClose()
{
  OpenIoState s(orphan->ioRef, closeState);

  return (0 == orphan->dirEntry->close(s));
}

PT_THREAD(FcVersion::connectionReset())
{
  struct pt *pt = &connResetState;

  PT_BEGIN(pt);

  /* flush all working requests for this connection */
  resetReq = workQueue;
  while (resetReq != 0)
  {
    nextReset = resetReq->next;
    if (this != resetReq)
    {
      if (resetReq->fid && resetReq->fid->dirEntry)
      {
        OpenIoState s(resetReq->fid->ioRef, resetReq->op);
        resetReq->fid->dirEntry->flush(s);
      }

      delete resetReq;
    }
    resetReq = nextReset;
  }
  workQueue = 0;

  // clean fidpool
  if (fidPool)
  {
    while ( (orphan = Fid::getFirst(fidPool)) )
    {
      if (orphan->ioRef)
      {
        closeState = 0;
        PT_WAIT_WHILE(pt, doClose());
      }
    }
    Fid::destroyPool(fidPool, mm);
    fidPool = 0;
  }

  // create new FidPool
  fidPool = Fid::createPool(mm);

  if (handleNoMemError(fidPool))
  {
    PT_EXIT(pt);
  }

//  if (msize)
//  {
//    conn->dotu = dotu;
//    conn->fidpool = sp_fidpool_create();
//  }
//  conn->flags &= ~Creset;

  PT_END(pt);

  return 0;
}

}
