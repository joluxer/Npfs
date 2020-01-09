/*
 * Rwalk.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RWALK_H_
#define RWALK_H_

#include "Rmessage.h"
#include "Twalk.h"
#include "../NpfsConstants.h"

namespace Npfs
{

class Rwalk: public Npfs::Rmessage
{
public:
  Rwalk(const Npfs::Twalk& tref);
//  virtual ~Rwalk();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  uint16_t nwqids;
  NpQid wqids[MaxWalkElements];
};

}

#endif /* RWALK_H_ */
