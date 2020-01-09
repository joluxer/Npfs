/*
 * Tattach.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TATTACH_H_
#define TATTACH_H_

#include "Tmessage.h"

#include "../NpStr.h"

namespace Npfs
{

class Tattach: public Npfs::Tmessage
{
public:
  Tattach(MsgSerDes& io, uint16_t tag);
  virtual ~Tattach();

  uint32_t afid;
  NpStr uname;
  NpStr aname;
};

}

#endif /* TATTACH_H_ */
