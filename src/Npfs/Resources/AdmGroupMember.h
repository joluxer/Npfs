/*
 * AdmGroupMember.h
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#ifndef ADMGROUPMEMBER_H_
#define ADMGROUPMEMBER_H_

#include "MemoryManager.h"

namespace Npfs
{

class AdmUsersEntry;

/***
 * This class represents a group member of a group in the 9P2000 server.
 */
class AdmGroupMember
{
public:
  AdmGroupMember(AdmUsersEntry& user);
//  virtual ~AdmGroupMember();

  static void useMemory(MemoryManager* mm);

  void * operator new(size_t size);
  void operator delete(void * obj);

protected:
  AdmUsersEntry* user;
  AdmGroupMember* next;

  friend class AdmUsersEntry;
  friend class AdmGroup;

  static MemoryManager* mm;
};

inline
void AdmGroupMember::useMemory(MemoryManager* mm_)
{
  mm = mm_;
}

}

#endif /* ADMGROUPMEMBER_H_ */
