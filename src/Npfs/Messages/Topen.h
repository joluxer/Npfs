/*
 * Topen.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TOPEN_H_
#define TOPEN_H_

#include "Tmessage.h"

namespace Npfs
{

class Topen: public Npfs::Tmessage
{
public:
  Topen(MsgSerDes& io, uint16_t tag);
//  virtual ~Topen();

  uint8_t mode;
};

}

#endif /* TOPEN_H_ */
