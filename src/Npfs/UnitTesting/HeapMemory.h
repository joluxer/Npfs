/*
 * HeapMemory.h
 *
 *  Created on: 24.08.2012
 *      Author: jlode
 */

#ifndef HEAPMEMORY_H_
#define HEAPMEMORY_H_

#include "MemoryManager.h"

class HeapMemory: public MemoryManager
{
public:
//  HeapMemory();
//  virtual ~HeapMemory();

  virtual void* allocate(size_t size);
  virtual void  release(const void* mem);

};

#endif /* HEAPMEMORY_H_ */
