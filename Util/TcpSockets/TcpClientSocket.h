/*
 * TcpClientSocket.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef TCPCLIENTSOCKET_H_
#define TCPCLIENTSOCKET_H_

// Definition of the TcpClientSocket class

#include "TcpSocket.h"

#include "ByteStream.h"

class TcpClientSocket: public TcpSocket
{
public:
  TcpClientSocket();
  TcpClientSocket(const char* hostAddr, int port);
//  virtual ~TcpClientSocket() {}

  bool connect(const char* hostAddr, int port);
};

#endif /* TCPCLIENTSOCKET_H_ */
