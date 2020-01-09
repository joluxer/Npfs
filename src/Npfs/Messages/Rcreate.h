/*
 * Rcreate.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RCREATE_H_
#define RCREATE_H_

#include "Rmessage.h"
#include "Tcreate.h"

namespace Npfs
{

class Rcreate: public Npfs::Rmessage
{
public:
  Rcreate(const Npfs::Tcreate& tref);
//  virtual ~Rcreate();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpQid qid;
  uint32_t iounit;
};

}

#endif /* RCREATE_H_ */
