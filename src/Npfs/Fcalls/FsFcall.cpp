/*
 * FsFcall.cpp
 *
 *  Created on: 27.07.2012
 *      Author: jlode
 */

#include "FsFcall.h"

namespace Npfs
{

FsFcall::FsFcall(MemoryManager* mm, uint16_t tag, Fid * *const _fidPool)
: Fcall(mm, tag), fidPool(_fidPool)
{
  PT_INIT(&state);
}

//FsFcall::~FsFcall()
//{
//  // Auto-generated destructor stub
//}

Rmessage* FsFcall::execute()
{
  Rmessage* reply;

  if (PT_SCHEDULE(engine()))
  {
    // fcall not complete
    reply = 0;
  }
  else
  {
    // fcall complete, result available
    reply = rMessage;
    rMessage = 0; // must not deleted on delete of the Fcall
    if (fid)
    {
      fid->decRef();
    }
  }

  return reply;
}


}
