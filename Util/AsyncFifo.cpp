/*
 * AsyncFifo.cpp
 *
 */

#include "AsyncFifo.h"

AsyncFifo::AsyncFifo(ByteStream& backend)
: ByteStreamDecorator(backend),
  txDataReadyHandler(0)
{
  txBuffer.data = 0;
  txBuffer.putIdx = 0;
  txBuffer.getIdx = 0;
  txBuffer.fill = 0;
  txBuffer.size = 0;

  rxBuffer.data = 0;
  rxBuffer.putIdx = 0;
  rxBuffer.getIdx = 0;
  rxBuffer.fill = 0;
  rxBuffer.size = 0;
}

//AsyncFifo::~AsyncFifo()
//{
//  // Auto-generated destructor stub
//}

void AsyncFifo::tellTxDataAvailable()
{
  if (txDataReadyHandler)
    txDataReadyHandler->send(true);
}

void AsyncFifo::tellTxBufferDrained()
{
  if (txDataReadyHandler)
    txDataReadyHandler->send(false);
}

AsyncFifo& AsyncFifo::assignTxBufferSpace(unsigned char* buffer, unsigned bufferSize)
{
  if (not buffer)
    bufferSize = 0;

  txBuffer.fill = 0;
  txBuffer.putIdx = 0;
  txBuffer.getIdx = 0;
  txBuffer.size = bufferSize;
  txBuffer.data = buffer;

  return *this;
}

AsyncFifo& AsyncFifo::assignRxBufferSpace(unsigned char* buffer, unsigned bufferSize)
{
  if (not buffer)
    bufferSize = 0;

  rxBuffer.fill = 0;
  rxBuffer.putIdx = 0;
  rxBuffer.getIdx = 0;
  rxBuffer.data = buffer;
  rxBuffer.size = bufferSize;


  return *this;
}

void AsyncFifo::sendDownstream()
{
  _Buffer& b = txBuffer;

  if (b.data)
  {
    unsigned count = b.fill;

    if (count)
    {
      if (count > (b.size - b.getIdx))
        count = (b.size - b.getIdx);

      count = backendIo.writeBlock(b.data + b.getIdx, count);

      b.getIdx = (b.getIdx + count) % b.size;
      b.fill -= count;
    }
    else
      tellTxBufferDrained();
  }
  else
    tellTxBufferDrained();
}

void AsyncFifo::receiveUpstream()
{
  _Buffer& b = rxBuffer;

  if (b.data)
  {
    unsigned room = b.size - b.fill;

    if (room)
    {
      unsigned count;

      if (room > (b.size - b.putIdx))
        room = (b.size - b.putIdx);

      if (room)
      {
        count = backendIo.readBlock(b.data + b.putIdx, room);

        b.putIdx = (b.putIdx + count) % b.size;
        b.fill += count;
      }
      else
      {
        // Daten verwerfen
        unsigned char dummy;
        backendIo.read(dummy);
      }
    }
    else
    {
      // Daten verwerfen
      unsigned char dummy;
      backendIo.read(dummy);
    }
  }
}

unsigned AsyncFifo::write(unsigned char byte)
{
  _Buffer& b = txBuffer;
  unsigned count = 0;

  if (b.data)
  {
    unsigned room = b.size - b.fill;

    if (room)
    {
      b.data[b.putIdx] = byte;
      count = 1;

      b.putIdx = (b.putIdx + count) % b.size;
      b.fill += count;
    }
  }
  else
    count = backendIo.write(byte);

  if(count)
    tellTxDataAvailable();

  return count;
}

unsigned AsyncFifo::writeBlock(const unsigned char* blk, unsigned size)
{
  _Buffer& b = txBuffer;
  unsigned count = 0;

  if (b.data)
  {
    unsigned room = b.size - b.fill;

    if (room)
    {
      for(count = 0; room && size && (b.putIdx < b.size); room--, size--)
      {
        b.data[b.putIdx] = blk[count];
        b.putIdx = (b.putIdx + 1) % b.size;
        count++;
        b.fill++;
      }
    }
  }
  else
    count = backendIo.writeBlock(blk, size);

  if(count)
    tellTxDataAvailable();

  return count;
}

unsigned AsyncFifo::read(unsigned char& byte)
{
  _Buffer& b = rxBuffer;
  unsigned count = 0;

  if (b.data)
  {
    if (b.fill)
    {
      byte = b.data[b.getIdx];
      count = 1;
      b.getIdx = (b.getIdx + count) % b.size;
      b.fill -= count;
    }
  }
  else
    count = backendIo.read(byte);

  return count;
}

unsigned AsyncFifo::readBlock(unsigned char* blk, unsigned size)
{
  _Buffer& b = rxBuffer;
  unsigned count = 0;

  if (b.data)
  {
    for(count = 0; b.fill && size; b.fill--, size--)
    {
      blk[count] = b.data[b.getIdx];
      count++;
      b.getIdx = (b.getIdx + 1) % b.size;
    }
  }
  else
    count = backendIo.readBlock(blk, size);

  return count;
}


unsigned AsyncFifo::remainingRxSpace() const
{
  unsigned remain;

  remain = rxBuffer.size - rxBuffer.fill;

  return remain;
}

unsigned AsyncFifo::remainingTxSpace() const
{
  unsigned remain;

  remain = txBuffer.size - txBuffer.fill;

  return remain;
}
