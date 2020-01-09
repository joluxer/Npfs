/*
 * FcWrite.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCWRITE_H_
#define FCWRITE_H_

#include "FsFcall.h"

#include "../Messages/Twrite.h"
#include "../Messages/Rwrite.h"

namespace Npfs
{

class FcWrite: public Npfs::FsFcall
{
public:
  FcWrite(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool, uint32_t msize);
  virtual ~FcWrite();

protected:
  const Twrite *const tMsg;
  const uint32_t msize;
  Rwrite* rMsg;
  uint64_t writeOffset;

  virtual
  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCWRITE_H_ */
