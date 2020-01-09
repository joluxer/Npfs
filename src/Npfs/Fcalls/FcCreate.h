/*
 * FcCreate.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FCCREATE_H_
#define FCCREATE_H_

#include "FsFcall.h"

#include "../Messages/Tcreate.h"
#include "../Messages/Rcreate.h"

namespace Npfs
{

class FcCreate: public Npfs::FsFcall
{
public:
  FcCreate(MemoryManager* mm, const Tmessage* tc, Fid * *const fidPool);
  virtual ~FcCreate();

protected:
  const Tcreate *const tMsg;
  Rcreate* rMsg;

  virtual
  PT_THREAD(engine());
  bool runOp();

};

}

#endif /* FCCREATE_H_ */
