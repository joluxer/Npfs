/*
 * FsFcall.h
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#ifndef FSFCALL_H_
#define FSFCALL_H_

#include "Fcall.h"
#include "pt/pt.h"

namespace Npfs
{

class FsFcall: public Npfs::Fcall
{
public:
  FsFcall(MemoryManager* mm, uint16_t tag, Fid * *const fidPool);
//  virtual ~FsFcall();

  virtual
  Rmessage* execute();

protected:
  Fid * *const fidPool;
  struct pt state;

  virtual
  PT_THREAD(engine()) =0;
};

}

#endif /* FSFCALL_H_ */
