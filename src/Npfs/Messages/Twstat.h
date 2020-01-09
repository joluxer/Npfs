/*
 * Twstat.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TWSTAT_H_
#define TWSTAT_H_

#include "Tmessage.h"
#include "../NpStat.h"

namespace Npfs
{

class Twstat: public Npfs::Tmessage
{
public:
  Twstat(MsgSerDes& io, uint16_t tag);
//  virtual ~Twstat();

  NpStat stat;
};

}

#endif /* TWSTAT_H_ */
