/*
 * LinuxNpfsTcpServer.cpp
 *
 *  Created on: 04.10.2012
 *      Author: lode
 */

#include "LinuxNpfsTcpServer.h"

#include "LinuxNpfsTcpConnection.h"

#include <stdio.h>
#include <stdlib.h>

LinuxNpfsTcpServer::LinuxNpfsTcpServer(int portNum, unsigned _msize, Npfs::Server& resServer, bool& _triggerServerFlag)
: serverSocket(portNum), npfsServer(resServer), msize(_msize), triggerServerFlag(_triggerServerFlag)
{
//  serverSocket.setNoDelay();
  if (serverSocket.hasPosixError())
  {
    perror("Error creating server socket");
    exit(-1);
  }
  waitForRead = true;
  waitForWrite = false;
}

//LinuxNpfsTcpServer::~LinuxNpfsTcpServer()
//{
//  // Auto-generated destructor stub
//}

void LinuxNpfsTcpServer::destroy()
{
  waitForRead = false;
  waitForWrite = false;
  removeFromChain();
  serverSocket.disconnect();
}

int LinuxNpfsTcpServer::fd()
{
  return serverSocket.fd();
}

void LinuxNpfsTcpServer::execute(bool readable, bool writable)
{
  if (!serverSocket.hasPosixError())
  {
    if (readable)
    {
      // der Normalfall ist eine neue Verbindung, die akzeptiert werden muss
      LinuxNpfsTcpConnection* newConn = new LinuxNpfsTcpConnection(npfsServer, triggerServerFlag);

      newConn->limitMaxMsgSize(msize);

      if (serverSocket.accept(newConn->getSocket()))
      {
        // das neue Objekt der gleichen Objekt-Kette hinzufügen, in der Server lebt
        newConn->addTo(*chain);
      }
      else
      {
        if (serverSocket.hasPosixError())
        {
          perror("Error accepting client connection");
        }
        else
        {
          fprintf(stderr, "Error accepting client connection: did not accept, reason unknown\n");
        }
        delete newConn;
      }
    }
  }
  else
  {
    perror("Server socket error");
  }
}
