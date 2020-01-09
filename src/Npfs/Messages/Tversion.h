/*
 * Tversion.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TVERSION_H_
#define TVERSION_H_

#include "Tmessage.h"
#include "../NpStr.h"

namespace Npfs
{

class Tversion: public Npfs::Tmessage
{
public:
  Tversion(MsgSerDes& io, uint16_t tag);
  virtual ~Tversion();

  uint32_t mSize;
  NpStr version;

  bool is9P2000() const;
};

}

#endif /* TVERSION_H_ */
