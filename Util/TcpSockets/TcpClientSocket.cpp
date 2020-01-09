/*
 * TcpClientSocket.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

// Implementation of the TcpClientSocket class

#include "TcpClientSocket.h"

TcpClientSocket::TcpClientSocket()
{
  create();
}

TcpClientSocket::TcpClientSocket(const char* hostAddr, int port)
: TcpClientSocket()
{
  connect(hostAddr, port);
}

bool TcpClientSocket::connect(const char* hostAddr, int port)
{
  bool success = isValid();

  if (!success)
    success = create();

  success &= TcpSocket::connect(hostAddr, port);

  return success;
}
