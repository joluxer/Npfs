/*
 * TransactionFile.tpl
 *
 *  Created on: 02.10.2012
 *      Author: lode
 */

#include "TransactionFile.h"

namespace Npfs
{

template<class SerDes>
MemoryManager* TransactionFile<SerDes>::mm = 0;

template<class SerDes>
TransactionFile<SerDes>::TransactionFile(const char* filename, Npfs::Directory* parent, uint16_t umask)
: Resource(filename, parent),
  beginTxnHandler(0), beginTxnRef(reinterpret_cast<void*>(~uintptr_t(0))),
  abortTxnHandler(0), abortTxnRef(reinterpret_cast<void*>(~uintptr_t(0))),
  commitTxnHandler(0), commitTxnRef(reinterpret_cast<void*>(~uintptr_t(0))),
  endTxnHandler(0), endTxnRef(reinterpret_cast<void*>(~uintptr_t(0)))
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
void TransactionFile<SerDes>::serialize(IoBuffer* ioRef)
{
  SerDes serdes(ioRef->transientValue);

  ioRef->bufferFill = serdes.serializeTo(ioRef->ioBuffer, SerDes::DataLength_bytes);
}

template<class SerDes>
PT_THREAD(TransactionFile<SerDes>::openCoRoutine(ResultMessage& result, Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit))
{
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  struct pt *pt = &static_cast<CoRoState*>(workRef.opState)->ptState;
  ResultMessage txnError = 0;
  auto oMode = mode & 0x03;

  ioRef->flag.wasOpenedForWrite = ((oMode == Owrite) || (oMode == Ordwr));

  PT_BEGIN(pt);

  if (not ioRef)
  {
    result = ErrorString::InternalServerError;
    break;
  }

  if (beginTxnHandler)
    PT_WAIT_UNTIL(pt, beginTxnHandler->execute(ioRef->transientValue, oMode, txnError, beginTxnRef));

  if (txnError)
  {
    result = txnError;
    PT_EXIT(pt);
  }

  ioRef->flag.isWritten = false;
  if (((mode & 0x03) == Owrite) || (mode & Otrunc))
  {
    ioRef->bufferFill = 0;
    ioRef->ioBuffer[0] = 0;
  }
  else
    serialize(ioRef);

  result = OpSuccess;

  PT_END(pt);

  return PT_ENDED;
}

template<class SerDes>
Npfs::Resource::ResultMessage TransactionFile<SerDes>::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  assert(0 != mm);
  ResultMessage result = 0;

  do // no goto, just break;
  {
    if (not workRef.ioState)
    {
      IoBuffer* ioRef = new(mm) IoBuffer;

      if (handleNoMemory(ioRef, result))
        break;

      if (handleNoMemory(ioRef->ioBuffer, result))
        break;

      workRef.ioState = ioRef;
    }

    if (not workRef.opState)
    {
      auto op = new (mm) CoRoState;

      if (handleNoMemory(op, result))
        break;

      PT_INIT(&op->ptState);
      workRef.opState = op;
    }

    openCoRoutine(result, workRef, mode, iounit);

    if (result)
    {
      if (OpSuccess != result)
      {
        delete workRef.ioState;
        workRef.ioState = nullptr;
      }

      delete workRef.opState;
      workRef.opState = nullptr;
    }
  } while (false);

  return result;
}

template<class SerDes>
Npfs::Resource::ResultMessage TransactionFile<SerDes>::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
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
Npfs::Resource::ResultMessage TransactionFile<SerDes>::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
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
void TransactionFile<SerDes>::flush(Npfs::OpenIoState& workRef)
{
  if (abortTxnHandler)
  {
    IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
    const char* dummyResult;
    abortTxnHandler->execute(ioRef->transientValue, ioRef->flag.wasOpenedForWrite, dummyResult, abortTxnRef);
  }

  if (workRef.ioState)
  {
    delete workRef.ioState;
    workRef.ioState = 0;
  }
}

template<class SerDes>
bool TransactionFile<SerDes>::deserialize(IoBuffer* ioRef)
{
  SerDes serdes(ioRef->transientValue);
  ioRef->ioBuffer[ioRef->bufferFill] = 0; // have guaranteed termination of the buffer, i.e. for conversion from ascii
  return serdes.deserializeFrom(ioRef->ioBuffer, ioRef->bufferFill);
}

template<class SerDes>
PT_THREAD(TransactionFile<SerDes>::closeCoRoutine(ResultMessage& result, Npfs::OpenIoState& workRef))
{
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  struct pt *pt = &static_cast<CoRoState*>(workRef.opState)->ptState;
  ResultMessage txnError = 0;

  PT_BEGIN(pt);

  if (ioRef->flag.isWritten && (0 != ioRef->bufferFill))
  {
    if (not deserialize(ioRef))
    {
      // have a deserialization error
      result = Npfs::ErrorString::Eio;
      PT_EXIT(pt);
    }
  }

  if (commitTxnHandler and ioRef->flag.wasOpenedForWrite)
    PT_WAIT_UNTIL(pt, commitTxnHandler->execute(ioRef->transientValue, ioRef->flag.isWritten, txnError, commitTxnRef));

  if (txnError)
  {
    result = txnError;
    PT_EXIT(pt);
  }

  // continue closing
  if (endTxnHandler)
    PT_WAIT_UNTIL(pt, endTxnHandler->notify(ioRef->transientValue, ioRef->flag.wasOpenedForWrite, txnError, endTxnRef));

  if (txnError)
  {
    result = txnError;
    PT_EXIT(pt);
  }

  result = OpSuccess;

  PT_END(pt);

  return PT_ENDED;
}

template<class SerDes>
Npfs::Resource::ResultMessage TransactionFile<SerDes>::close(Npfs::OpenIoState& workRef)
{
  ResultMessage result = 0;

  do // no goto, just break
  {
    if (not workRef.opState)
    {
      auto op = new (mm) CoRoState;

      if (handleNoMemory(op, result))
        break;

      PT_INIT(&op->ptState);
      workRef.opState = op;
    }

    closeCoRoutine(result, workRef);

    if (result)
    {
      delete workRef.ioState;
      workRef.ioState = 0;
      delete workRef.opState;
      workRef.opState = 0;
    }
  } while (false);

  return result;
}

template<class SerDes>
Npfs::Resource::ResultMessage TransactionFile<SerDes>::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLen)
{
  ResultMessage result = Npfs::ErrorString::Enospace;

  if (newLen <= SerDes::DataLength_bytes)
  {
    result = OpSuccess;
  }

  return result;
}

template<class SerDes>
TransactionFile<SerDes>::IoBuffer::IoBuffer()
{
  ioBuffer = reinterpret_cast<unsigned char*>(npfsNew(SerDes::DataLength_bytes + 1, mm));
}

template<class SerDes>
TransactionFile<SerDes>::IoBuffer::~IoBuffer()
{
  if (ioBuffer)
    npfsDelete(ioBuffer);
}

}
