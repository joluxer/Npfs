/*
 * Twrite.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TWRITE_H_
#define TWRITE_H_

#include "Tmessage.h"

namespace Npfs
{

class Twrite: public Npfs::Tmessage
{
public:
  Twrite(MsgSerDes& io, uint16_t tag);
  virtual ~Twrite();

  int64_t offset;
  uint32_t count;
  unsigned char* data;
};

}

#endif /* TWRITE_H_ */
