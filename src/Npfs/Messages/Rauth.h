/*
 * Rauth.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RAUTH_H_
#define RAUTH_H_

#include "Rmessage.h"

#include "Tauth.h"

namespace Npfs
{

class Rauth: public Npfs::Rmessage
{
public:
  Rauth(const Npfs::Tauth& tref);
//  virtual ~Rauth();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpQid aqid;
};

}

#endif /* RAUTH_H_ */
