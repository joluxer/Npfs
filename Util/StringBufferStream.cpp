/*
 * StringBufferStream.cpp
 *
 *  Created on: 03.12.2011
 *      Author: lode
 */

#include "StringBufferStream.h"

StringBufferStream::StringBufferStream()
: writeBuffer(0), readBuffer(0)
{}

StringBufferStream::StringBufferStream(unsigned char* buffer, unsigned bufferSize)
{
  setBigReadBuffer(buffer, bufferSize);
  setBigWriteBuffer(buffer, bufferSize);
}

StringBufferStream::StringBufferStream(unsigned char* writeBuffer, unsigned writeBufferSize, unsigned char* readBuffer, unsigned readBufferSize)
{
  setBigWriteBuffer(writeBuffer, writeBufferSize);
  setBigReadBuffer(readBuffer, readBufferSize);
}

//StringBufferStream::~StringBufferStream()
//{
//  // Auto-generated destructor stub
//}

unsigned StringBufferStream::read(unsigned char &b)
{
  if (readPosition < readSize)
  {
    b = readBuffer[readPosition];
    readPosition++;
    return 1;
  }
  return 0;
}

unsigned StringBufferStream::write(unsigned char b)
{
  if (writePosition < writeSize)
  {
    writeBuffer[writePosition] = b;
    writePosition++;
    return 1;
  }
  return 0;
}

unsigned StringBufferStream::bytesWriteable() const
{
  return (writeSize - writePosition);
}

unsigned StringBufferStream::bytesReadable(void) const
{
  return (readSize - readPosition);
}

void StringBufferStream::setBigWriteBuffer(unsigned char *bigBuffer, unsigned bufferSize)
{
  writeBuffer = bigBuffer;
  writeSize = bufferSize;
}

void StringBufferStream::setBigReadBuffer(unsigned char *bigBuffer, unsigned bufferSize)
{
  readBuffer = bigBuffer;
  readSize = bufferSize;
}
