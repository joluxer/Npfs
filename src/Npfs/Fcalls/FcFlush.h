/*
 * FcFlush.h
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#ifndef FCFLUSH_H_
#define FCFLUSH_H_

#include "Fcall.h"

#include "../Messages/Tflush.h"

namespace Npfs
{

class FcFlush: public Npfs::Fcall
{
public:
  FcFlush(MemoryManager* mm, const Tmessage* tc, Fcall* &wq);
  virtual ~FcFlush();

  virtual
  Rmessage* execute();

protected:
  const Tflush *const tMsg;

  Fcall* &workQueue;
};

}

#endif /* FCFLUSH_H_ */
