/*
 * NpUser.h
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#ifndef NPUSER_H_
#define NPUSER_H_

#include "NpStr.h"
#include "NpStr.h"

#include <stdint.h>

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers.
 */
class NpUser
{
public:
  NpUser();
//  virtual ~NpUser();

  operator NpStr() const;

  bool operator ==(const NpStr& other) const;

  const char * uname;
  NpUser*  dfltgroup;
  unsigned ngroups;
  NpUser* groups;

  NpUser*   next;
};

}

#endif /* NPUSER_H_ */
