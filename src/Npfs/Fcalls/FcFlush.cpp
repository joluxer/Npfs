/*
 * FcFlush.cpp
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#include "FcFlush.h"

#include "../Messages/Rflush.h"

namespace Npfs
{

FcFlush::FcFlush(MemoryManager* mm, const Tmessage* tc, Fcall* &wq)
: Fcall(mm, tc->tag()), tMsg(static_cast<const Tflush*>(tc)), workQueue(wq)
{}

FcFlush::~FcFlush()
{
  delete tMsg;
}

Rmessage* FcFlush::execute()
{
  Rflush* reply = new(mm) Rflush(*tMsg);
  rMessage = reply;

  if (!handleNoMemError(reply))
  {
    uint16_t oldtag;
    Fcall *creq;

    oldtag = tMsg->oldTag;

    for(creq = workQueue; creq != 0; creq = creq->next)
    {
      if (creq->tag == oldtag)
      {
        if (creq->fid)
        {
          if (creq->fid->dirEntry)
          {
            OpenIoState s(creq->fid->ioRef, creq->op);
            creq->fid->dirEntry->flush(s);
          }
          creq->fid->decRef();          // this FID is no longer pending
        }
        if (creq == workQueue)
        {
          workQueue = creq->next;
        }
        if (this != creq)
          delete creq;
        break;
      }
    }
  }

  Rmessage* result = rMessage;
  rMessage = 0;

  return result;
}

}
