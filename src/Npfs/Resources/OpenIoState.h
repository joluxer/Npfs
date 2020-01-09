/*
 * OpenIoState.h
 *
 */

#ifndef OPENIOSTATE_H_
#define OPENIOSTATE_H_

#include "../Resources/IoState.h"
#include "../Resources/BlockingOpState.h"

#include "MemoryManager.h"

namespace Npfs
{

/***
 * This class is an internal data representation for the 9P2000 server.
 *
 * It hold vital references to the state of resource operations.
 */
class OpenIoState
{
public:
  OpenIoState(IoState* &ioReference, BlockingOpState* &opRef);
//  virtual ~OpenIoState();

  IoState* &ioState;  // State with FID lifetime (from attach/walk till clunk/remove)
  BlockingOpState* &opState; // State with Fcall lifetime (from Tmsg in till Rmsg out)
};

}

#endif /* OPENIOSTATE_H_ */
