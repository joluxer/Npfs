/*
 * ByteStreamConnection.h
 *
 *  Created on: 02.07.2012
 *      Author: lode
 */

#ifndef BYTESTREAMCONNECTION_H_
#define BYTESTREAMCONNECTION_H_

#include "Connection.h"

#include "ByteStream.h"
#include "BufferedIo.h"

namespace Npfs
{

/***
 * Diese Klasse stellt die Verbindung zwischen einem seriell orientierten
 * Interface (ByteStream) und dem Server her.
 *
 * Es wird ein ByteStream mit BufferedIo-Interface benötigt, da die Connection
 * Informationen über die Füllhöhe der I/O-Buffer im RX- und TX-Pfad benötigt.
 */

class ByteStreamConnection: public Npfs::Connection
{
public:
  ByteStreamConnection();
//  virtual
//  ~ByteStreamConnection();

  void setIo(unsigned mbufSize, ByteStream* interface, BufferedIo* ioBuffer);

  virtual unsigned maxMsgSize() const;
protected:
  ByteStream* ioData;
  BufferedIo* ioBuffer;
  unsigned mSizeMax;

  virtual unsigned bytesReadable();
  virtual unsigned bytesWritable();

  virtual unsigned readData(unsigned char* buf, unsigned length);
  virtual void startTxPacket();
  virtual unsigned sendData(const unsigned char* buf, unsigned length);
  virtual void finishTxPacket();

  virtual bool driveTransmitter();  ///< return true, if TX is pending
  virtual void doAliveCheck();
};

inline
void ByteStreamConnection::setIo(unsigned mbufSize, ByteStream* interface, BufferedIo* iobuf)
{
  mSizeMax = mbufSize;
  ioData = interface;
  ioBuffer = iobuf;
}

}

#endif /* BYTESTREAMCONNECTION_H_ */
