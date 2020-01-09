/*
 * BlockingOpState.h
 *
 */

#ifndef BLOCKINGOPSTATE_H_
#define BLOCKINGOPSTATE_H_

#include "MemoryManager.h"
#include "../NpfsNew.h"

namespace Npfs
{

/***
 * This is the base class for implementing state holding objects, that
 * are used in blocking operations on resources.
 *
 * These objects are used per Fcall/operation.
 *
 * If a operation will block, then the implementation must ensure, that
 * it will only block logically, i.e. will return immediately with some sort
 * of state held in a BlockingOpState object, waiting for to be called again, to
 * continue its work in the next call.
 *
 * The calls to blocking operations are repeated, until the operation completes, so
 * the scheduling for the operation is ensured. The call frequency is the call
 * frequency to Server::operate() of the parent 9P2000 server object.
 *
 * This is intended to be used in conjunction with ProtoThreads.
 */
class BlockingOpState
{
public:
//  BlockingOpState();
  virtual ~BlockingOpState();

  void * operator new(size_t size, MemoryManager* mm) throw();
  void operator delete(void * mem) throw();

};

inline
void * BlockingOpState::operator new(size_t size, MemoryManager* mm) throw()
{
  return npfsNew(size, mm);
}

inline
void BlockingOpState::operator delete(void *mem) throw()
{
  npfsDelete(mem);
}

}

#endif /* BLOCKINGOPSTATE_H_ */
