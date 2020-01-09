/*
 * Rwrite.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RWRITE_H_
#define RWRITE_H_

#include "Rmessage.h"
#include "Twrite.h"

namespace Npfs
{

class Rwrite: public Npfs::Rmessage
{
public:
  Rwrite(const Npfs::Twrite& tref);
//  virtual ~Rwrite();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  uint32_t count;
};

}

#endif /* RWRITE_H_ */
