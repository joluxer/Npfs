/*
 * Tread.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TREAD_H_
#define TREAD_H_

#include "Tmessage.h"

namespace Npfs
{

class Tread: public Npfs::Tmessage
{
public:
  Tread(MsgSerDes& io, uint16_t tag);
//  virtual ~Tread();

  int64_t offset;
  uint32_t count;
};

}

#endif /* TREAD_H_ */
