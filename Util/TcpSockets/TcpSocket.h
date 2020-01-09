/*
 * TcpSocket.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

// Definition of the TcpSocket class

#include <arpa/inet.h>

#include "Util/FdIoStream.h"

class TcpSocket: public FdIoStream
{
public:
  TcpSocket();
  virtual ~TcpSocket();

  bool isConnected();
  void disconnect();

  // ByteStream Interface
//  virtual unsigned write(unsigned char b);
//  virtual unsigned read(unsigned char &b);
  virtual unsigned writeBlock(const unsigned char *b, unsigned numBytes);
  virtual unsigned readBlock(unsigned char *b, unsigned numBytes);

protected:
  sockaddr_in socketAddr;
  bool socketIsConnectedListeningAccepted:1;

  bool create();

  // Server initialization
  bool bind(const int port);
  bool listen(int backlog = 5);
  bool accept(TcpSocket& newSocket);
  bool setBlocking(bool block);

  // Client initialization
  bool connect(const char* hostAddr, const int port);

//  // Data Transmission
  unsigned send(const unsigned char* buffer, unsigned bLength); // is always non-blocking
  unsigned recv(unsigned char* buffer, unsigned bLength);       // is always non-blocking

  bool isValid() const
  {
    return sysFd > -1;
  }
};

inline
bool TcpSocket::isConnected()
{
  return isValid() && socketIsConnectedListeningAccepted;
}

#endif /* TCPSOCKET_H_ */
