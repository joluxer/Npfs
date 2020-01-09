/*
 * TcpSocket.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

// Implementation of the TcpSocket class.


#include "TcpSocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <assert.h>

#include <stdio.h>

TcpSocket::TcpSocket()
: socketIsConnectedListeningAccepted(false)
{

  memset(&socketAddr, 0, sizeof(socketAddr));

}

TcpSocket::~TcpSocket()
{
  disconnect();
}

bool TcpSocket::create()
{
  bool success = false;

  myErrno = 0;
  sysFd = ::socket(AF_INET, SOCK_STREAM, 0);

  if (!isValid())
  {
    myErrno = errno;
  }
  else
  {
    // errors during TIME_WAIT period on pending sockets, that where not closed gracefully - argh
    int on = 1;
    if (::setsockopt(sysFd, SOL_SOCKET, SO_REUSEADDR, (const void*) &on, sizeof(on)) == -1)
    {
      myErrno = errno;
      ::close(sysFd);
      sysFd = -1;
    }
    else
    {
      myErrno = 0;
      success = true;
    }
  }

  return success;
}

void TcpSocket::disconnect()
{
  if (isValid())
    ::close(sysFd);
  sysFd = -1;
  myErrno = 0;
}

bool TcpSocket::bind(const int port)
{
  bool success = false;

  if (isValid())
  {
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = INADDR_ANY;
    socketAddr.sin_port = htons(port);

    int bind_return = ::bind(sysFd, (struct sockaddr *) &socketAddr, sizeof(socketAddr));

    if (bind_return == -1)
    {
      myErrno = errno;
    }
    else
    {
      myErrno = 0;
      success = true;
    }
  }

  return success;
}

bool TcpSocket::listen(int backlog)
{
  bool success = false;

  if (isValid())
  {
    int listen_return = ::listen(sysFd, backlog);

    if (listen_return == -1)
    {
      myErrno = errno;
    }
    else
    {
      myErrno = 0;
      success = true;
      socketIsConnectedListeningAccepted = true;
    }
  }

  return success;
}

bool TcpSocket::accept(TcpSocket& new_socket)
{
  bool success = false;
  int addr_length = sizeof(new_socket.socketAddr);

  int newFd = ::accept(sysFd, (sockaddr *) &new_socket.socketAddr, (socklen_t *) &addr_length);

  if (newFd == -1)
  {
    if (EWOULDBLOCK != errno)
    {
      myErrno = errno;
    }
  }
  else
  {
    myErrno = 0;
    new_socket.setFd(newFd);
    new_socket.socketIsConnectedListeningAccepted = true;
    success = true;
  }

  return success;
}

bool TcpSocket::setBlocking(bool block)
{
  bool success = false;

  int status = ::fcntl(sysFd, F_GETFL);
  if (status == -1)
  {
    myErrno = errno;
  }
  else
  {
    if (block)
    {
      ::fcntl(sysFd, F_SETFL, status & ~O_NONBLOCK);
    }
    else
    {
      ::fcntl(sysFd, F_SETFL, status | O_NONBLOCK);
    }
    myErrno = 0;
    success = true;
  }

  return success;
}

bool TcpSocket::connect(const char* hostAddr, const int port)
{
  bool success = false;

  if (isValid())
  {
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);

    int status = ::inet_pton(AF_INET, hostAddr, &socketAddr.sin_addr);

    if (-1 == status)
    {
      assert(errno != EAFNOSUPPORT);
      myErrno = errno;
    }
    else if (status)
    {
      status = ::connect(sysFd, (sockaddr *) &socketAddr, sizeof(socketAddr));

      if (status == 0)
      {
        myErrno = 0;
        success = true;
        socketIsConnectedListeningAccepted = true;
      }
      else
      {
        myErrno = errno;
      }
    }
  }

  return success;
}

unsigned TcpSocket::send(const unsigned char* buffer, unsigned bLength)
{
  unsigned retVal = 0;

  if (this->isValid())
  {
    int status = ::send(sysFd, buffer, bLength, MSG_NOSIGNAL | MSG_DONTWAIT);

    if (status == -1)
    {
      if ((EAGAIN == errno) || (EWOULDBLOCK == errno))
      {
        retVal = 0;
      }
      else if ((ECONNRESET == errno) || (EPIPE == errno))
      {
        ::close(sysFd);
        sysFd = -1;
        retVal = 0;
        socketIsConnectedListeningAccepted = false;
      }
      else
      {
        myErrno = errno;
        retVal = 0;
      }
    }
    else
    {
      myErrno = 0;
      retVal = status;
    }
  }

  return retVal;
}

unsigned TcpSocket::recv(unsigned char* buffer, unsigned bLength)
{
  unsigned retVal = 0;

  if (isValid())
  {
    int status = ::recv(sysFd, buffer, bLength, MSG_NOSIGNAL | MSG_DONTWAIT);

    if (status == -1)
    {
      if ((EAGAIN == errno) || (EWOULDBLOCK == errno))
      {
        retVal = 0;
      }
      else if (ENOTCONN == errno)
      {
        ::close(sysFd);
        sysFd = -1;
        retVal = 0;
        socketIsConnectedListeningAccepted = false;
      }
      else
      {
        myErrno = errno;
        retVal = 0;
      }
    }
    else if (0 == status)
    {
      ::close(sysFd);
      sysFd = -1;
      retVal = 0;
      socketIsConnectedListeningAccepted = false;
    }
    else
    {
      myErrno = 0;
      retVal = status;
    }
  }

  return retVal;
}

unsigned TcpSocket::writeBlock(const unsigned char *b, unsigned numBytes)
{
  return send(b, numBytes);
}

unsigned TcpSocket::readBlock(unsigned char *b, unsigned numBytes)
{
  return recv(b, numBytes);
}

