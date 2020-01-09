/*
 * VariableFile.tpl
 *
 *  Created on: 02.10.2012
 *      Author: lode
 */

#include "VariableFile.h"

namespace Npfs
{

template<class SerDes>
MemoryManager* VariableFile<SerDes>::mm = 0;

template<class SerDes>
VariableFile<SerDes>::VariableFile(const char* filename, Npfs::Directory* parent, typename SerDes::VarType& backendVar, uint16_t umask)
: Resource(filename, parent), variable(backendVar), updateHandler(0), changeHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  if (SerDes::canSerialize)
  {
    // file is readable
    stat.mode |= 0444 & ~umask;
  }

  if (SerDes::canDeserialize)
  {
    // file is writable
    stat.mode |= 0222 & ~umask;
  }

  stat.length = SerDes::DataLength_bytes;
}

template<class SerDes>
Npfs::Resource::ResultMessage VariableFile<SerDes>::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = 0;
  assert(0 != mm);

  if (!updateHandler || updateHandler->execute(variable))
  {
    IoBuffer* ioRef = new(mm) IoBuffer;

    if (!handleNoMemory(ioRef, result))
    {
      workRef.ioState = ioRef;
      ioRef->flag.isWritten = false;
      ioRef->flag.isClosing = false;
      if (((mode & 0x03) == Owrite) || (mode & Otrunc))
      {
        ioRef->bufferFill = 0;
        ioRef->ioBuffer[0] = 0;
      }
      else
      {
        SerDes serdes(variable);

        serdes.serializeTo(ioRef->ioBuffer, SerDes::DataLength_bytes);
        ioRef->bufferFill = SerDes::DataLength_bytes;
      }

      result = OpSuccess;
    }
    else
    {
      result = NoMemory;
    }
  }

  return result;
}

template<class SerDes>
Npfs::Resource::ResultMessage VariableFile<SerDes>::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  unsigned maxBytes = 0;

  if (offset > ioRef->bufferFill)
    count = maxBytes = 0;
  else
    maxBytes = ioRef->bufferFill - offset;

  if (count > maxBytes)
    count = maxBytes;

  if (count)
    memcpy(dataBuffer, ioRef->ioBuffer + offset, count);

  return result;
}

template<class SerDes>
Npfs::Resource::ResultMessage VariableFile<SerDes>::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  unsigned maxBytes = 0;

  if (offset > SerDes::DataLength_bytes)
  {
    count = maxBytes = 0;
    result = Npfs::ErrorString::Enospace;
  }
  else
  {
    maxBytes = SerDes::DataLength_bytes - offset;
    if (offset >= ioRef->bufferFill)
      memset(ioRef->ioBuffer + offset, 0, maxBytes);
  }

  if (count > maxBytes)
  {
    count = 0;
    result = Npfs::ErrorString::Enospace;
  }

  if (count)
  {
    ioRef->flag.isWritten = true;
    memcpy(ioRef->ioBuffer + offset, dataBuffer, count);
    if ((offset + count) > ioRef->bufferFill)
      ioRef->bufferFill = offset + count;
    stat.qid.version++;
  }

  return result;
}

template<class SerDes>
void VariableFile<SerDes>::flush(Npfs::OpenIoState& workRef)
{
  if (workRef.ioState)
  {
    delete workRef.ioState;
    workRef.ioState = 0;
  }
}

template<class SerDes>
Npfs::Resource::ResultMessage VariableFile<SerDes>::close(Npfs::OpenIoState& workRef)
{
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);

  if (ioRef->flag.isWritten && (0 != ioRef->bufferFill))
  {
    if (!ioRef->flag.isClosing)
    {
      SerDes serdes(variable);

      ioRef->ioBuffer[ioRef->bufferFill] = 0; // have guaranteed termination of the buffer, i.e. for conversion from ascii
      if (serdes.deserializeFrom(ioRef->ioBuffer, ioRef->bufferFill))
      {
        if (!changeHandler || changeHandler->notify(variable))
        {
          delete ioRef;
          workRef.ioState = ioRef = 0;
        }
        else
        {
          ioRef->flag.isClosing = true;
          result = 0;
        }
      }
      else
      {
        delete ioRef;
        workRef.ioState = ioRef = 0;
        result = Npfs::ErrorString::Eio;
      }
    }
    else
    {
      // continue closing
      if (!changeHandler || changeHandler->notify(variable))
      {
        delete ioRef;
        workRef.ioState = ioRef = 0;
      }
      else
      {
        ioRef->flag.isClosing = true;
        result = 0;
      }
    }
  }
  else
  {
    delete ioRef;
    workRef.ioState = ioRef = 0;
  }

  return result;
}

template<class SerDes>
Npfs::Resource::ResultMessage VariableFile<SerDes>::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLen)
{
  ResultMessage result = Npfs::ErrorString::Enospace;

  if (newLen <= SerDes::DataLength_bytes)
  {
    result = OpSuccess;
  }

  return result;
}

}
