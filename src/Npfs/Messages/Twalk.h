/*
 * Twalk.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TWALK_H_
#define TWALK_H_

#include "Tmessage.h"
#include "../NpStr.h"
#include "../NpfsConstants.h"

namespace Npfs
{

class Twalk: public Npfs::Tmessage
{
public:
  Twalk(MsgSerDes& io, uint16_t tag);
  virtual ~Twalk();

  uint32_t newfid;
  uint16_t nWnames;
  NpStr wnames[MaxWalkElements];
};

}

#endif /* TWALK_H_ */
