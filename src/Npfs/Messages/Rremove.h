/*
 * Rremove.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RREMOVE_H_
#define RREMOVE_H_

#include "Rmessage.h"
#include "Tremove.h"

namespace Npfs
{

class Rremove: public Npfs::Rmessage
{
public:
  Rremove(const Npfs::Tremove& tref);
//  virtual ~Rremove();

  virtual void serialize(void);
};

}

#endif /* RREMOVE_H_ */
