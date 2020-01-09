/*
 * Fcall.cpp
 *
 *  Created on: 19.07.2012
 *      Author: jlode
 */

#include "Fcall.h"

namespace Npfs
{

Fcall::Fcall(MemoryManager* mem, uint16_t _tag)
: tag(_tag), op(0), fid(0), rMessage(0),
  previous(0), next(0),
  mm(mem)
{
}

Fcall::~Fcall()
{
  if (previous)
  {
    previous->next = next;
  }

  if (next)
  {
    next->previous = previous;
  }

  next = 0;
  previous = 0;

  if (rMessage)
  {
    // Rmessage is not returned to the Server, so delete it
    if ((typeof(rMessage))~0 != rMessage)
      delete rMessage;
  }
}

void Fcall::createError(const Tmessage* tMessage, const char* errMessage)
{
  if ((typeof(rMessage))~0 != rMessage)
  {
    delete rMessage;
    rMessage = new(mm) Rerror(*tMessage, errMessage);
  }

}

bool Fcall::handleNoMemError(void* mem)
{
  if (mem)
  {
    return false;
  }

  if ((typeof(rMessage))~0 != rMessage)
    delete rMessage;

  rMessage = (typeof(rMessage))~0;

  return true;
}

bool Fcall::handleNoMemResult(Resource::ResultMessage msg)
{
  if (Resource::NoMemory != msg)
  {
    return false;
  }

  if ((typeof(rMessage))~0 != rMessage)
    delete rMessage;

  rMessage = (typeof(rMessage))~0;

  return true;
}

}
