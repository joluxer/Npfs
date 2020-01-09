/*
 * Rversion.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RVERSION_H_
#define RVERSION_H_

#include "Rmessage.h"
#include "Tversion.h"

namespace Npfs
{

class Rversion: public Npfs::Rmessage
{
public:
  Rversion(const Npfs::Tversion& tref);
//  virtual ~Rversion();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  uint32_t mSize;
  NpStr version;
};

}

#endif /* RVERSION_H_ */
