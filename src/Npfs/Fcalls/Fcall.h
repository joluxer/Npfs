/*
 * Fcall.h
 *
 *  Created on: 19.07.2012
 *      Author: jlode
 */

#ifndef FCALL_H_
#define FCALL_H_

#include "../Messages/Rmessage.h"
#include "../Messages/Tmessage.h"
#include "../Fid.h"
#include "../Resources/Resource.h"
#include "../Connection.h"
#include "../Resources/BlockingOpState.h"
#include "MemoryManager.h"

namespace Npfs
{

class Fcall
{
public:
  Fcall(MemoryManager* mm, uint16_t tag);
  virtual ~Fcall();

  const uint16_t tag;
  BlockingOpState* op;
  Fid* fid;
  Rmessage* rMessage;

  virtual
  Rmessage* execute() =0;

  Fcall *previous, *next;

  void * operator new(size_t size, MemoryManager* mm) throw();
  void operator delete(void * mem) throw();

protected:
  MemoryManager* mm;
  void createError(const Tmessage* tMessage, const char* errMessage);
  bool handleNoMemError(void* mem); ///< return false, if there was no error and no error handling occurred
  bool handleNoMemResult(Resource::ResultMessage res); ///< return false, if there was no error and no error handling occurred
};

inline
void * Fcall::operator new(size_t size, MemoryManager* mm) throw()
{
  return npfsNew(size, mm);
}

inline
void Fcall::operator delete(void *mem) throw()
{
  npfsDelete(mem);
}

}

#endif /* FCALL_H_ */
