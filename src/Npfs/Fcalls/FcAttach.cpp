/*
 * FcAttach.cpp
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#include "FcAttach.h"

#include "../Messages/Rattach.h"
#include "../NpfsConstants.h"
#include "../Resources/AdmUsersFile.h"

#include <assert.h>

namespace Npfs
{

FcAttach::FcAttach(MemoryManager* mm, const Tmessage* tc, Fid* *const _fidPool, ServerNamespace* ns)
: Fcall(mm, tc->tag()), tMsg(static_cast<const Tattach*>(tc)), fidPool(_fidPool), srvNamespaces(ns)
{
  assert(srvNamespaces);
}

FcAttach::~FcAttach()
{
  delete tMsg;
}

Rmessage* FcAttach::execute()
{
  Rattach* reply = new(mm) Rattach(*tMsg);
  rMessage = reply;

  if (!handleNoMemError(reply))
  {
    Fid *afid;

    do  // this is just a breakable block to avoid goto...
    {
      fid = Fid::find(fidPool, tMsg->fid());
      if (fid)
      {
        createError(tMsg, ErrorString::Einuse);
        break;
      }

      fid = Fid::create(fidPool, tMsg->fid(), mm);
      if (handleNoMemError(fid))
        break;
      else
        fid->incRef();

      if (tMsg->afid != Fid::NoFid)
      {
        afid = Fid::find(fidPool, tMsg->afid);
        if (!afid)
        {
          createError(tMsg, ErrorString::Eunknownfid);
          break;
        }
        else
        {
          if (!afid->isAuth())
          {
            createError(tMsg, ErrorString::Ebadusefid);
            break;
          }
          // TODO: check for correctly authenticated afid
          afid->incRef();
        }
      }

      // check user name
      fid->user = AdmUsersFile::getNpUser(tMsg->uname);
      if (!fid->user)
      {
        createError(tMsg, ErrorString::Eunknownuser);
        break;
      }

      // find server name space
      ServerNamespace* ns = srvNamespaces->searchFor(tMsg->aname);
      if (ns)
      {
        fid->directory = ns->attach(tMsg->aname);
        fid->dirEntry = fid->directory.get();
        fid->nsDetach = &ns->detachHandler;
      }
      else
      {
        createError(tMsg, ErrorString::Enotfound);
        break;
      }

      if (fid->directory)
      {
        reply->qid = fid->directory->qid();
        fid->incRef();
      }
      else
      {
        delete fid;
        createError(tMsg, ErrorString::Enotfound);
        break;
      }

    } while (false);
  }

  Rmessage* result = rMessage;
  rMessage = 0;

  return result;
}

}
