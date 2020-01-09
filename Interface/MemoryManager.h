/*
 * MemoryManager.h
 *
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include <stdlib.h>


/**
 * This class represents a simple memory manager interface.
 *
 * Implementors of this interface simply provide chunks of memory from an pool.
 */
class MemoryManager
{
public:
//  MemoryManager();
  virtual ~MemoryManager();

  virtual void* allocate(size_t) = 0;
  virtual void  release(const void*) = 0;

};

#endif /* MEMORYMANAGER_H_ */
