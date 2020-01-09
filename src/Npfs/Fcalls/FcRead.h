/*
 * FcRead.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCREAD_H_
#define FCREAD_H_

#include "FsFcall.h"
#include "../Messages/Tread.h"
#include "../Messages/Rread.h"

namespace Npfs
{

class FcRead: public Npfs::FsFcall
{
public:
  FcRead(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool, uint32_t msize);
  virtual ~FcRead();

protected:
  const Tread *const tMsg;
  Rread* rMsg;
  const uint32_t msize;

  virtual
  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCREAD_H_ */
