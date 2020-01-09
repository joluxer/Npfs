/*
 * FcAttach.h
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#ifndef FCATTACH_H_
#define FCATTACH_H_

#include "Fcall.h"

#include "../Messages/Tattach.h"
#include "../Resources/ServerNamespace.h"

namespace Npfs
{

class FcAttach: public Npfs::Fcall
{
public:
  FcAttach(MemoryManager*mm, const Tmessage* tc, Fid* *const fidPool, ServerNamespace* ns);
  virtual ~FcAttach();

  virtual
  Rmessage* execute();

protected:
  const Tattach *const tMsg;
  Fid* *const fidPool;
  ServerNamespace* srvNamespaces;
};

}

#endif /* FCATTACH_H_ */
