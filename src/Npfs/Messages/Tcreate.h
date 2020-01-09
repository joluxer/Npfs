/*
 * Tcreate.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TCREATE_H_
#define TCREATE_H_

#include "Tmessage.h"
#include "../NpStr.h"

namespace Npfs
{

class Tcreate: public Npfs::Tmessage
{
public:
  Tcreate(MsgSerDes& io, uint16_t tag);
  virtual ~Tcreate();

  NpStr name;
  int32_t perm;
  uint8_t mode;
};

}

#endif /* TCREATE_H_ */
