/*
 * Rstat.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RSTAT_H_
#define RSTAT_H_

#include "Rmessage.h"
#include "Tstat.h"

namespace Npfs
{

class Rstat: public Npfs::Rmessage
{
public:
  Rstat(const Npfs::Tstat& tref);
//  virtual ~Rstat();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpStat stat;
};

}

#endif /* RSTAT_H_ */
