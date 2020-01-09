/*
 * IoState.h
 *
 *  Created on: 03.06.2012
 *      Author: lode
 */

#ifndef IOSTATE_H_
#define IOSTATE_H_

#include <stdint.h>

#include "../NpfsNew.h"

namespace Npfs
{

/***
 * This is the base class for implementing state holding objects, that
 * are used by open resources per FID.
 *
 * If the open state of a resource needs some state data to be held, then it must
 * be held in an implementation of this base class.
 */
class IoState
{
public:
//  IoState();
//  virtual ~IoState();

  void* operator new(size_t size, MemoryManager* mm) throw();
  void operator delete(void *mem) throw();
};

inline
void* IoState::operator new(size_t size, MemoryManager* mm) throw()
{
  return npfsNew(size, mm);
}

inline
void IoState::operator delete(void *mem) throw()
{
  npfsDelete(mem);
}

}

#endif /* IOSTATE_H_ */
