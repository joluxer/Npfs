/*
 * MsgBufferSerDes.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "MsgBufferSerDes.h"

#include <string.h>

namespace Npfs
{

MsgBufferSerDes::MsgBufferSerDes(unsigned char* _readBuffer, unsigned char* _writeBuffer, unsigned _bufferLength)
: readBuffer(_readBuffer), writeBuffer(_writeBuffer), bufferLength(_bufferLength), writePos(0)
{}

//MsgBufferSerDes::~MsgBufferSerDes()
//{
//  // Auto-generated destructor stub
//}

//void MsgBufferSerDes::reset()
//{
//  rbyteCount = 0;
//  writePos = 0;
//}

unsigned MsgBufferSerDes::readData(unsigned char* buffer, unsigned readLength)
{
  if (readBuffer)
  {
    if ((rbyteCount + readLength) > bufferLength)
    {
      readLength = bufferLength - rbyteCount;
    }

    if (readLength)
    {
      memcpy(buffer, readBuffer + rbyteCount, readLength);
//      rbyteCount += readLength; --> nicht hier ausführen, wird von der Elternklasse gepflegt
    }
  }
  else
  {
    readLength = 0;
  }

  return readLength;
}

unsigned MsgBufferSerDes::sendData(const unsigned char* buffer, unsigned writeLength)
{
  if (writeBuffer)
  {
    if ((writePos + writeLength) > bufferLength)
    {
      writeLength = bufferLength - writePos;
    }

    if (writeLength)
    {
      memcpy(writeBuffer + writePos, buffer, writeLength);
      writePos += writeLength;
    }
  }
  else
  {
    writeLength = 0;
  }

  return writeLength;
}

}
