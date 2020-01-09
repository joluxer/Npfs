/*
 * AdmGroup.h
 *
 *  Created on: 10.06.2012
 *      Author: lode
 */

#ifndef SYSTEMGROUP_H_
#define SYSTEMGROUP_H_

#include "AdmUsersEntry.h"

namespace Npfs
{

/***
 * This class represents a group of users in the 9P2000 server.
 */
class AdmGroup
{
public:
  AdmGroup(const NpUser& group);
  AdmGroup(const NpStr& gid);
//  virtual ~AdmGroup();

  bool hasGroupLeader(const NpUser& user) const;

  bool hasMember(const NpUser& user) const;

protected:
  AdmUsersEntry* usersEntry;

};

}

#endif /* SYSTEMGROUP_H_ */
