/*
 * AsyncFifoBuffer.h
 *
 */

#ifndef ASYNCFIFOBUFFER_H_
#define ASYNCFIFOBUFFER_H_

#include "AsyncFifo.h"
#include "BufferedIo.h"

/**
 * Diese Klasse stellt zusätzlich zu den Eigenschaften der Elternklasse AsyncFifo das BufferedIo-Inteface
 * zur Verfügung.
 */
class AsyncFifoBuffer: public AsyncFifo, public BufferedIo
{
public:
  AsyncFifoBuffer(ByteStream& backend);
//  virtual ~AsyncFifoBuffer();

  virtual
  unsigned bytesWritable() const;
  virtual
  unsigned bytesReadable(void) const;
  virtual
  bool flush(void);

  virtual
  void setBigWriteBuffer(unsigned char *bigBuffer, unsigned bufferSize);
  virtual
  void setBigReadBuffer(unsigned char *bigBuffer, unsigned bufferSize);
};

#endif /* ASYNCFIFOBUFFER_H_ */
