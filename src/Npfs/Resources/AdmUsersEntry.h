/*
 * AdmUsersEntry.h
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#ifndef ADMUSERSENTRY_H_
#define ADMUSERSENTRY_H_

#include "AdmGroupMember.h"
#include "../NpUser.h"

namespace Npfs
{

/***
 * This class represents an entry in the /adm/users file of the 9P2000 server.
 *
 * Such entries serve both purposes: to identify a user and to identify a group.
 * See http://man.cat-v.org/plan_9/6/users for more information of such entries.
 */
class AdmUsersEntry
{
public:
  AdmUsersEntry(const char* name, AdmUsersEntry* previousEntry = 0, AdmUsersEntry* groupLeader = (AdmUsersEntry*)~0);
  virtual ~AdmUsersEntry();

  AdmUsersEntry& appendToList(AdmUsersEntry& nUser);  ///< this appends the user to the chain of entries the given object, normally used inside the server code

  AdmUsersEntry& addMember(AdmUsersEntry& user);  ///< this appends a new group member

  AdmUsersEntry& setGroupLeader(AdmUsersEntry* newLeader); ///< this sets a new group leader, or removes it, if zero
  AdmUsersEntry* groupLeader() const;

  operator NpUser();
  bool operator==(const AdmUsersEntry& other) const;
  bool operator!=(const AdmUsersEntry& other) const;

protected:
  const char *const name;
  AdmUsersEntry* leader;
  AdmGroupMember* member;

  NpUser groupData;

  unsigned stringLength;  ///< this member tells, how long the formatted string of the entry will be, a helper for the AdmUsersFile class

  AdmUsersEntry* next;

  unsigned calcStringLength();  ///< this method calculates the current string length and caches the value in stringLength
  unsigned printLine(char* buffer, uint32_t bufferSize, unsigned offset); ///< this method prints the string into the buffer with offset into the string

  static
  void copyString(const char* src, char* destBuffer, unsigned destSize, unsigned& destIdx, unsigned& offset); ///< this static method copies string data from src to dest (null terminated and size limited)

  friend class AdmUsersFile;
  friend class AdmGroup;
};

inline
AdmUsersEntry& AdmUsersEntry::setGroupLeader(AdmUsersEntry* newLeader)  ///< this sets a new group leader, or removes it, if empty
{
  leader = newLeader;

  return *this;
}

inline
AdmUsersEntry* AdmUsersEntry::groupLeader() const
{
  return leader;
}

//inline
//AdmUsersEntry& AdmUsersEntry::copyGroupData(NpUser& _groupData)
//{
//  _groupData = groupData;
//
//  return *this;
//}

inline
AdmUsersEntry::operator NpUser()
{
  return groupData;
}

}

#endif /* ADMUSERSENTRY_H_ */
