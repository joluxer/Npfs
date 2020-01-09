/*
 * NpfsNew.cpp
 *
 *  Created on: 07.12.2012
 *      Author: jlode
 */

#include "NpfsNew.h"

namespace Npfs
{

void* npfsNew(size_t size, MemoryManager* mm)
{
  // move mem-ptr out of the allocated block?
  MemoryManager* *mem = reinterpret_cast<MemoryManager**>(mm->allocate(size + sizeof(mm)));

  if (mem)
  {
    *mem = mm;
    mem++;  // this results possibly in a misalignment of the returned memory block
  }

  return reinterpret_cast<void*>(mem);
}

void npfsDelete(void * mem_)
{
  if (mem_)
  {
    MemoryManager** mem = reinterpret_cast<MemoryManager**>(mem_) - 1;

    (*mem)->release(mem);
  }
}


}  // namespace Npfs
