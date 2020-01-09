/*
 * Rattach.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RATTACH_H_
#define RATTACH_H_

#include "Rmessage.h"

#include "Tattach.h"

namespace Npfs
{

class Rattach: public Npfs::Rmessage
{
public:
  Rattach(const Npfs::Tattach& tref);
//  virtual ~Rattach();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpQid qid;
};

}

#endif /* RATTACH_H_ */
