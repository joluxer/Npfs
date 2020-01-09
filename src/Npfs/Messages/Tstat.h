/*
 * Tstat.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TSTAT_H_
#define TSTAT_H_

#include "Tmessage.h"

namespace Npfs
{

class Tstat: public Npfs::Tmessage
{
public:
  Tstat(MsgSerDes& io, uint16_t tag);
//  virtual ~Tstat();
};

}

#endif /* TSTAT_H_ */
