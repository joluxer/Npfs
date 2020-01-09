/*
 * FcAuth.h
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#ifndef FCAUTH_H_
#define FCAUTH_H_

#include "Fcall.h"

#include "../Messages/Tauth.h"

namespace Npfs
{

class FcAuth: public Npfs::Fcall
{
public:
  FcAuth(MemoryManager* mm, const Tmessage* tc);
  virtual ~FcAuth();

  virtual
  Rmessage* execute();

protected:
  const Tauth *const tMsg;
};

}

#endif /* FCAUTH_H_ */
