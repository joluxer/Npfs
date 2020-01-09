/*
 * AdmGroupMember.cpp
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#include "AdmGroupMember.h"

#include <cassert>

namespace Npfs
{

MemoryManager* AdmGroupMember::mm = 0;

AdmGroupMember::AdmGroupMember(AdmUsersEntry& _user)
: user(&_user), next(0)
{}

//AdmGroupMember::~AdmGroupMember()
//{
//  // Auto-generated destructor stub
//}

void * AdmGroupMember::operator new(size_t size)
{
  assert(mm && "need to set a MemoryManager for AdmGroupMember suitable for small objects");
  return mm->allocate(size);
}

void AdmGroupMember::operator delete(void * obj)
{
  assert(mm && "need to set a MemoryManager for AdmGroupMember suitable for small objects");
  mm->release(obj);
}

}
