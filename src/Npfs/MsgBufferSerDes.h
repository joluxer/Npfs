/*
 * MsgBufferSerDes.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef MSGBUFFERSERDES_H_
#define MSGBUFFERSERDES_H_

#include "MsgSerDes.h"

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers,
 * die Hilfsfunktionalität für die Abläufe des Servers und für Tests implementiert.
 */

class MsgBufferSerDes: public Npfs::MsgSerDes
{
public:
  MsgBufferSerDes(unsigned char* readBuffer, unsigned char* writeBuffer, unsigned bufferLength);
//  virtual ~MsgBufferSerDes();

  void reset();

  unsigned bytesReadable() const;
  unsigned bytesWritable() const;

protected:
  unsigned char* readBuffer;
  unsigned char* writeBuffer;
  unsigned bufferLength;
  unsigned writePos;
  virtual unsigned readData(unsigned char* readBuffer, unsigned readLength);
  virtual unsigned sendData(const unsigned char* writeBuffer, unsigned writeLength);

};

inline
unsigned MsgBufferSerDes::bytesReadable() const
{
  return (bufferLength - rbyteCount);
}

inline
unsigned MsgBufferSerDes::bytesWritable() const
{
  return (bufferLength - writePos);
}

inline
void MsgBufferSerDes::reset()
{
  rbyteCount = 0;
  writePos = 0;
}

}

#endif /* MSGBUFFERSERDES_H_ */
