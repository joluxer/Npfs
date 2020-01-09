/*
 * AdmGroup.cpp
 *
 *  Created on: 10.06.2012
 *      Author: lode
 */

#include "AdmGroup.h"

#include "AdmUsersFile.h"

namespace Npfs
{

AdmGroup::AdmGroup(const NpUser& group)
: usersEntry(AdmUsersFile::getEntry(group))
{}

AdmGroup::AdmGroup(const NpStr& group)
: usersEntry(AdmUsersFile::getEntry(group))
{}

//AdmGroup::~AdmGroup()
//{
//  // Auto-generated destructor stub
//}

bool AdmGroup::hasGroupLeader(const NpUser& _user) const
{
  bool isLeader = false;
  AdmUsersEntry* user;

  // query AdmUsersFile for user and check, if it is the group leader or, in case of no group leader, check for group membership
  if (usersEntry && (user = AdmUsersFile::getEntry(_user)))
  {
    AdmUsersEntry* e;
    e = usersEntry->groupLeader();
    if (e)
    {
      if (e == user)
      {
        isLeader = true;
      }
      else
      {
        isLeader = this->hasMember(_user);
      }
    }
  }

  return isLeader;
}

bool AdmGroup::hasMember(const NpUser& _user) const
{
  bool isMember = false;

  if (usersEntry)
  {
    // check group membership
    AdmGroupMember* m = usersEntry->member;

    while (m)
    {
      if (_user == m->user->name)
      {
        isMember = true;
        break;
      }
      m = m->next;
    }
  }

  return isMember;
}


}
