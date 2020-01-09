/*
 * Rwstat.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RWSTAT_H_
#define RWSTAT_H_

#include "Rmessage.h"
#include "Twstat.h"

namespace Npfs
{

class Rwstat: public Npfs::Rmessage
{
public:
  Rwstat(const Npfs::Twstat& tref);
//  virtual ~Rwstat();

  virtual void serialize(void);
};

}

#endif /* RWSTAT_H_ */
