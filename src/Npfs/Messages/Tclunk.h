/*
 * Tclunk.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TCLUNK_H_
#define TCLUNK_H_

#include "Tmessage.h"

namespace Npfs
{

class Tclunk: public Npfs::Tmessage
{
public:
  Tclunk(MsgSerDes& io, uint16_t tag);
//  virtual ~Tclunk();

};

}

#endif /* TCLUNK_H_ */
