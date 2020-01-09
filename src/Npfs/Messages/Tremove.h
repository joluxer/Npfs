/*
 * Tremove.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TREMOVE_H_
#define TREMOVE_H_

#include "Tmessage.h"

namespace Npfs
{

class Tremove: public Npfs::Tmessage
{
public:
  Tremove(MsgSerDes& io, uint16_t tag);
//  virtual ~Tremove();
};

}

#endif /* TREMOVE_H_ */
