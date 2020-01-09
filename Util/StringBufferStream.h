/*
 * StringBufferStream.h
 *
 *  Created on: 03.12.2011
 *      Author: lode
 */

#ifndef STRINGBUFFERSTREAM_H_
#define STRINGBUFFERSTREAM_H_

#include "ByteStream.h"
#include "BufferedIo.h"
#include "SeekStream.h"

class StringBufferStream: public ByteStream, public BufferedIo, public SeekStream
{
public:
  StringBufferStream();
  StringBufferStream(unsigned char* buffer, unsigned bufferSize = (unsigned)-1);
  StringBufferStream(unsigned char* writeBuffer, unsigned writeBufferSize, unsigned char* readBuffer, unsigned readBufferSize);
//  virtual ~StringBufferStream();

  // ByteStream Interface
  virtual unsigned read(unsigned char &b);
  virtual unsigned write(unsigned char b);
  // ggf kann man die Block-Routinen beschleunigen,
  // indem man sie direkt implementiert, nicht den default aus ByteStream verwendet

  // BufferedIo Interface
  virtual unsigned bytesWriteable() const;
  virtual unsigned bytesReadable(void) const;
  virtual void setBigWriteBuffer(unsigned char *bigBuffer, unsigned bufferSize);
  virtual void setBigReadBuffer(unsigned char *bigBuffer, unsigned bufferSize);

  // SeekStream Interface
  // die Default-Implementationen sind hinreichend.

protected:
  unsigned char* writeBuffer, *readBuffer;
};

#endif /* STRINGBUFFERSTREAM_H_ */
