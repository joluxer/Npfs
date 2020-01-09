/*
 * ByteStreamConnection.cpp
 *
 *  Created on: 02.07.2012
 *      Author: lode
 */

#include "ByteStreamConnection.h"

#include <assert.h>

namespace Npfs
{

ByteStreamConnection::ByteStreamConnection()
: ioData(0), ioBuffer(0), mSizeMax(0)
{}

//ByteStreamConnection::~ByteStreamConnection()
//{
//  // Auto-generated destructor stub
//}

unsigned ByteStreamConnection::readData(unsigned char* buf, unsigned length)
{
  return ioData->readBlock(buf, length);
}

void ByteStreamConnection::startTxPacket()
{
  // intentionally left empty
}


unsigned ByteStreamConnection::sendData(const unsigned char* buf, unsigned length)
{
  unsigned retVal = ioData->writeBlock(buf, length);

  return retVal;
}

void ByteStreamConnection::finishTxPacket()
{
  // intentionally left empty
}

bool ByteStreamConnection::driveTransmitter()
{
  // intentionally left empty
  return false;
}

unsigned ByteStreamConnection::maxMsgSize() const
{
  return mSizeMax;
}

unsigned ByteStreamConnection::bytesReadable()
{
  return ioBuffer->bytesReadable();
}

unsigned ByteStreamConnection::bytesWritable()
{
  return ioBuffer->bytesWritable();
}

void ByteStreamConnection::doAliveCheck()
{
  connectionAlive = true;
}

}
