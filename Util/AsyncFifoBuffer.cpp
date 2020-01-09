/*
 * AsyncFifoBuffer.cpp
 *
 */

#include "AsyncFifoBuffer.h"

AsyncFifoBuffer::AsyncFifoBuffer(ByteStream& backend)
: AsyncFifo(backend)
{}

//AsyncFifoBuffer::~AsyncFifoBuffer()
//{
//  // Auto-generated destructor stub
//}

unsigned AsyncFifoBuffer::bytesWritable() const
{
  return remainingTxSpace();
}

unsigned AsyncFifoBuffer::bytesReadable(void) const
{
  return rxBuffer.fill;
}

bool AsyncFifoBuffer::flush(void)
{
  sendDownstream();

  bool buffersNotEmpty = true;

  buffersNotEmpty &= (0 != txBuffer.fill);

  return buffersNotEmpty;
}

void AsyncFifoBuffer::setBigWriteBuffer(unsigned char *bigBuffer, unsigned bufferSize)
{
  assignTxBufferSpace(bigBuffer, bufferSize);
}

void AsyncFifoBuffer::setBigReadBuffer(unsigned char *bigBuffer, unsigned bufferSize)
{
  assignRxBufferSpace(bigBuffer, bufferSize);
}
