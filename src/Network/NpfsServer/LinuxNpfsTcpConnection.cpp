/*
 * LinuxNpfsTcpConnection.cpp
 *
 *  Created on: 04.10.2012
 *      Author: lode
 */

#include "LinuxNpfsTcpConnection.h"

#include <stdio.h>

#include <assert.h>

//extern int debug;

LinuxNpfsTcpConnection::LinuxNpfsTcpConnection(Npfs::Server& resServer, bool& triggerServerFlag)
: ioBuffer(socket), txAvailableHandler(this, &LinuxNpfsTcpConnection::startAsyncTx),
  npfsServer(resServer), triggerServer(triggerServerFlag)
{
  ioBuffer.setBigWriteBuffer(txBuffer, IoBufferSize);
  ioBuffer.setBigReadBuffer(rxBuffer, IoBufferSize);
  ioBuffer.connectTxAvailableSignal(&txAvailableHandler);
  bufferMaxMsgSize = ioBuffer.bytesWritable();  // RX und TX müssen symmetrisch sein, sonst haut das mit der MSIZE-Angabe für den Server nicht mehr hin

  npfsServer.connect(*this);

  waitForRead = true;
  waitForWrite = false;
}

//LinuxNpfsTcpConnection::~LinuxNpfsTcpConnection()
//{
//  // Auto-generated destructor stub
//}

void LinuxNpfsTcpConnection::destroy()
{
  waitForRead = false;
  waitForWrite = false;
  removeFromChain();
  // Connection vom Server entfernen
  npfsServer.disconnect(*this);
  socket.disconnect();
  // dieses Objekt löschen
  delete this;
}

int LinuxNpfsTcpConnection::fd()
{
  return socket.fd();
}

void LinuxNpfsTcpConnection::execute(bool readable, bool writable)
{
  if (writable && socket.isConnected())
  {
    ioBuffer.sendDownstream();
  }

  // nun die empfangenen Daten verarbeiten
  if (readable && socket.isConnected())
  {
    ioBuffer.receiveUpstream();
    triggerServer = true;
  }

  if (!socket.isConnected())
  {
    destroy();
  }
}

void LinuxNpfsTcpConnection::startAsyncTx(bool txReady)
{
  waitForWrite = txReady;
}

unsigned LinuxNpfsTcpConnection::bytesReadable()
{
  return ioBuffer.bytesReadable();
}

unsigned LinuxNpfsTcpConnection::bytesWritable()
{
  return ioBuffer.bytesWritable();
}

unsigned LinuxNpfsTcpConnection::readData(unsigned char* buf, unsigned length)
{
  return ioBuffer.readBlock(buf, length);
}

void LinuxNpfsTcpConnection::startTxPacket()
{
  // intentionally left empty
}

unsigned LinuxNpfsTcpConnection::sendData(const unsigned char* buf, unsigned length)
{
  return ioBuffer.writeBlock(buf, length);
}

void LinuxNpfsTcpConnection::finishTxPacket()
{
  ioBuffer.sendDownstream();
}

bool LinuxNpfsTcpConnection::driveTransmitter()   ///< return true, if TX is pending
{
  ioBuffer.sendDownstream();

  return waitForWrite;
}

void LinuxNpfsTcpConnection::doAliveCheck()
{
  connectionAlive = socket.isConnected();
}

unsigned LinuxNpfsTcpConnection::maxMsgSize() const
{
  return bufferMaxMsgSize;
}

void LinuxNpfsTcpConnection::limitMaxMsgSize(unsigned msize)
{
  if (msize < bufferMaxMsgSize)
    bufferMaxMsgSize = msize;
}
