/*
 * HeapMemory.cpp
 *
 *  Created on: 24.08.2012
 *      Author: jlode
 */

#include "HeapMemory.h"

#include <stdlib.h>

//HeapMemory::HeapMemory()
//{
//  // Auto-generated constructor stub
//
//}
//
//HeapMemory::~HeapMemory()
//{
//  // Auto-generated destructor stub
//}

void* HeapMemory::allocate(size_t size)
{
  void* mem = malloc(size);

  return mem;
}

void  HeapMemory::release(const void* mem)
{
  ::free((void *)mem);
}

