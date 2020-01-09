/*
 * TcpServerSocket.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

// Implementation of the TcpServerSocket class

#include "TcpServerSocket.h"

TcpServerSocket::TcpServerSocket(int port)
{
  create();

  if (!myErrno)
  {
    bind(port);

    if (!myErrno)
    {
      listen();
    }
  }

}

TcpServerSocket::~TcpServerSocket()
{}
