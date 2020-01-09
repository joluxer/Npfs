/*
 * FcVersion.h
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#ifndef FCVERSION_H_
#define FCVERSION_H_

#include "Fcall.h"
#include "../Messages/Tversion.h"
#include "../Messages/Rversion.h"

#include "pt/pt.h"

namespace Npfs
{

class FcVersion: public Npfs::Fcall
{
public:
  FcVersion(MemoryManager* mm, const Tmessage* tc, Fcall* &wq, Fid** &fidPool, Connection& conn);
  virtual ~FcVersion();

  virtual
  Rmessage* execute();

protected:
  const Tversion *const tmsg;
  Rversion* rMsg;
  Fcall* &workQueue;
  Fid** &fidPool;
  Connection& conn;

  struct pt fcState, connResetState;
  Fcall *resetReq, *nextReset;
  Fid* orphan;
  BlockingOpState* closeState;

  PT_THREAD(engine());
  PT_THREAD(connectionReset());
  bool doClose();

};

}

#endif /* FCVERSION_H_ */
