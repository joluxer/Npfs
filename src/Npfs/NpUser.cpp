/*
 * NpUser.cpp
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#include "NpUser.h"

#include <string.h>

namespace Npfs
{

NpUser::NpUser()
: uname(0), dfltgroup(this), ngroups(0), groups(0),
  next(0)
{

}

//NpUser::~NpUser()
//{
//  // Auto-generated destructor stub
//}

NpUser::operator NpStr() const
{
  return NpStr(uname);
}

bool NpUser::operator ==(const NpStr& other) const
{
  bool equal = true;

  equal = (strlen(uname) == other.len) && (0 == memcmp(uname, other.str, other.len));

  return equal;
}

}
