/*
 * FcAuth.cpp
 *
 *  Created on: 26.07.2012
 *      Author: jlode
 */

#include "FcAuth.h"

#include "../Messages/Rauth.h"

namespace Npfs
{

FcAuth::FcAuth(MemoryManager* mm, const Tmessage* tc)
: Fcall(mm, tc->tag()), tMsg(static_cast<const Tauth*>(tc))
{}

FcAuth::~FcAuth()
{
  delete tMsg;
}

Rmessage* FcAuth::execute()
{
  // TODO: handle afid

  createError(tMsg, "auth not supported");

  Rmessage* reply = rMessage;
  rMessage = 0;
  return reply;
}

}
