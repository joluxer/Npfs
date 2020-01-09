/*
 * TcpServerSocket.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef TCPSERVERSOCKET_H_
#define TCPSERVERSOCKET_H_

// Definition of the TcpServerSocket class

#include "TcpSocket.h"

class TcpServerSocket: public TcpSocket
{
public:

  TcpServerSocket(int port);
  virtual ~TcpServerSocket();

  bool setBlocking(bool block = true);

  bool accept(TcpSocket& new_socket);

};

inline
bool TcpServerSocket::setBlocking(bool block)
{
  return TcpSocket::setBlocking(block);
}

inline
bool TcpServerSocket::accept(TcpSocket& new_socket)
{
  return TcpSocket::accept(new_socket);
}

#endif /* TCPSERVERSOCKET_H_ */
