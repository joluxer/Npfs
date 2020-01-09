/*
 * Ropen.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef ROPEN_H_
#define ROPEN_H_

#include "Rmessage.h"
#include "Topen.h"

namespace Npfs
{

class Ropen: public Npfs::Rmessage
{
public:
  Ropen(const Npfs::Topen& tref);
//  virtual ~Ropen();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpQid qid;
  uint32_t iounit;
};

}

#endif /* ROPEN_H_ */
