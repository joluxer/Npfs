/*
 * Server.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "Server.h"

#include "Fcalls/FcVersion.h"
#include "Fcalls/FcAuth.h"
#include "Fcalls/FcAttach.h"
#include "Fcalls/FcFlush.h"
#include "Fcalls/FcWalk.h"
#include "Fcalls/FcOpen.h"
#include "Fcalls/FcCreate.h"
#include "Fcalls/FcRead.h"
#include "Fcalls/FcWrite.h"
#include "Fcalls/FcClunk.h"
#include "Fcalls/FcRemove.h"
#include "Fcalls/FcStat.h"
#include "Fcalls/FcWstat.h"

#include <assert.h>

namespace Npfs
{

Server::Server(MemoryManager* memman)
: mm(memman), connections(0), orphanedFid(0), srvNamespaces(0)
{
  orphanedFidPool = Fid::createPool(mm);
}

Server::~Server()
{
  assert(0 == connections);
  assert(0 == orphanedFid);

  if (orphanedFidPool)
  {
    Fid::destroyPool(orphanedFidPool, mm);
  }
}

void Server::serve(ServerNamespace& ns)
{
  if (!srvNamespaces)
  {
    srvNamespaces = &ns;
  }
  else
  {
    srvNamespaces->append(ns);
  }
}

bool Server::connect(Connection& conn)
{
  bool success = false;
  conn.connectMemMan(mm);
  ConnectionListElement* cListElement = new(mm) ConnectionListElement(conn);
  if (0 != cListElement)
  {
    if (connections)
      connections->previous = cListElement;

    cListElement->next = connections;
    connections = cListElement;
    cListElement->previous = 0;
    success = true;
  }

  return success;
}

void Server::disconnect(Connection& conn)
{
  ConnectionListElement* cListElement = connections;

  while (cListElement)
  {
    // wenn gefunden, aus Kette entfernen
    if (&conn == &cListElement->io)
    {
      if (connections == cListElement)
      {
        connections = cListElement->next;
      }
      if (cListElement->previous)
      {
        cListElement->previous->next = cListElement->next;
      }
      if (cListElement->next)
      {
        cListElement->next->previous = cListElement->previous;
      }
      cListElement->previous = 0;
      cListElement->next = 0;

      // flush all Fcall's
      Fcall* fc = cListElement->workQueue, *next;

      while (fc)
      {
        if (fc->fid && fc->fid->dirEntry)
        {
          OpenIoState s(fc->fid->ioRef,fc->op);
          fc->fid->dirEntry->flush(s);
        }

        next = fc->next;
        delete fc;
        fc = next;
        cListElement->workQueue = fc;
      }

      //  move open Fid's to some close queue
      if (cListElement->fidPool)
      {
        Fid* orphan;
        while ( (orphan = Fid::getFirst(cListElement->fidPool)) )
        {
          orphan->moveTo(orphanedFidPool);
        }

        Fid::destroyPool(cListElement->fidPool, mm);
      }

      conn.connectMemMan(0);
      delete cListElement;
      break;
    }
    cListElement = cListElement->next;
  }
}

bool Server::operate()
{
  pollConnections();
  return processWorkQueues();
}

bool Server::scheduleTx()
{
  bool needRecall = false;
  // for all connections:
  ConnectionListElement* conn;
  for (conn = connections; conn; conn = conn->next)
  {
    // try to send
    needRecall |= conn->io.runTxMsgQueue();
  }

  return needRecall;
}

void Server::pollConnections()
{
  // for all connections:
  ConnectionListElement* conn;
  for (conn = connections; conn != 0; conn = conn->next)
  {
    // poll connection
    Tmessage* msg = conn->io.poll();
    if (msg)
    {
      // handle errors that are already raised
      const NpStr* err = msg->getError();

      if (err)
      {
        Rmessage* rMsg = new(mm) Npfs::Rerror(*msg, *err);
        if (rMsg)
        {
          conn->io.send(rMsg);
        }
        else
        {
          conn->io.sendNoMemError(msg->tag());
        }
        delete msg;
      }
      else
      {
        // queue work request for Tmessage
        Fcall *fc = createRequest(msg, conn);
        if (fc)
        {
          if (conn->workQueue)
            conn->workQueue->previous = fc;

          fc->next = conn->workQueue;
          conn->workQueue = fc;
          fc->previous = 0;
        }
        else
        {
          // send error message NoMem
          conn->io.sendNoMemError(msg->tag());
          // delete Tmessage
          delete msg;
        }
      }
    }
  }
}

bool Server::processWorkQueues()
{
  Fcall *req, *nextReq;
  bool pendingOps = false;

  // for all connections:
  ConnectionListElement* conn;
  for (conn = connections; conn; conn = conn->next)
  {
    // process all open work requests exactly one time
    for (req = conn->workQueue; req;)
    {
      if (processRequest(req, conn))
      {
        if (req == conn->workQueue)
        {
          conn->workQueue = req->next;
        }
        nextReq = req->next;  // der Next-Pointer geht jetzt verloren, also sichern, kann erst jetzt gesichert werden, weil das Processing eines FcFlush die Kette ändert...
        delete req;
        req = nextReq;
      }
      else
      {
        req = req->next;  // request läuft noch, nächsten
      }
    }
    pendingOps |= (0 != conn->workQueue);
  }


  if (!orphanedFid)
  {
    orphanedFid = Fid::getFirst(orphanedFidPool);
  }
  if (orphanedFid)
  {
    if (orphanedFid->dirEntry && orphanedFid->ioRef)
    {
      OpenIoState s(orphanedFid->ioRef, orphanCleanupState);
      if (0 != orphanedFid->dirEntry->close(s))
      {
        delete orphanedFid;
        orphanedFid = 0;
      }
    }
    else
    {
      delete orphanedFid;
      orphanedFid = 0;
    }
  }

  pendingOps |= (0 != orphanedFid);

  return pendingOps;
}

Fcall* Server::createRequest(Tmessage* tMsg, ConnectionListElement* conn)
{
  assert(0 != tMsg);
  assert(0 != conn);

  Fcall* reply = 0;

  switch (tMsg->type)
  {
  case Message::Tversion:
    reply = new(mm) FcVersion(mm, tMsg, conn->workQueue, conn->fidPool, conn->io);
    break;
  case Message::Tauth:
    reply = new(mm) FcAuth(mm, tMsg);
    break;
  case Message::Tattach:
    reply = new(mm) FcAttach(mm, tMsg, conn->fidPool, srvNamespaces);
    break;
  case Message::Tflush:
    reply = new(mm) FcFlush(mm, tMsg, conn->workQueue);
    break;
  case Message::Twalk:
    reply = new(mm) FcWalk(mm, tMsg, conn->fidPool);
    break;
  case Message::Topen:
    reply = new(mm) FcOpen(mm, tMsg, conn->fidPool);
    break;
  case Message::Tcreate:
    reply = new(mm) FcCreate(mm, tMsg, conn->fidPool);
    break;
  case Message::Tread:
    reply = new(mm) FcRead(mm, tMsg, conn->fidPool, conn->io.msize());
    break;
  case Message::Twrite:
    reply = new(mm) FcWrite(mm, tMsg, conn->fidPool, conn->io.msize());
    break;
  case Message::Tclunk:
    reply = new(mm) FcClunk(mm, tMsg, conn->fidPool);
    break;
  case Message::Tremove:
    reply = new(mm) FcRemove(mm, tMsg, conn->fidPool);
    break;
  case Message::Tstat:
    reply = new(mm) FcStat(mm, tMsg, conn->fidPool);
    break;
  case Message::Twstat:
    reply = new(mm) FcWstat(mm, tMsg, conn->fidPool);
    break;
  default:
    {
      bool unknownTmessageTypeShouldNeverHappen = false;
      assert(unknownTmessageTypeShouldNeverHappen);
      unknownTmessageTypeShouldNeverHappen = unknownTmessageTypeShouldNeverHappen;
      break;
    }
  }

  return reply;
}

bool Server::processRequest(Fcall* req, ConnectionListElement* conn)
{
  assert(0 != req);
  Rmessage* reply = 0;

  reply = req->execute();

  if ((typeof(reply))~0 == reply)
  {
    // all done, no memory
    conn->io.sendNoMemError(req->tag);
    return true;
  }
  else if (reply)
  {
    // all done, send good reply
    conn->io.send(reply);
    return true;
  }

  // call me again
  return false;
}

}
