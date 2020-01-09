/*
 * IoPinEdgeBlockingFile.cpp
 *
 *  Created on: 30.08.2015
 *      Author: lode
 */

#include "IoPinEdgeBlockingFile.h"
#include "BinaryStringSerDes.h"

#include <cassert>

namespace Npfs
{

namespace
{
const NpStr NoneStr{"none", 4};
const NpStr RisingStr{"rising", 6};
const NpStr FallingStr{"falling", 7};
const NpStr BothStr{"both", 4};
}

template<>
const BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::ValuePair BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::Values[] =
{
    { IoPinEdgeBlockingFile::NoEdgeTrigger, &NoneStr },
    { IoPinEdgeBlockingFile::RisingEdgeTrigger, &RisingStr },
    { IoPinEdgeBlockingFile::FallingEdgeTrigger, &FallingStr },
    { IoPinEdgeBlockingFile::BothEdgeTigger, &BothStr },
    { 0, nullptr },
};

template
const BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::ValuePair BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::Values[5];

template<>
const uint64_t BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::DataLength_bytes = 8;
template
const uint64_t BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::DataLength_bytes;

template<>
const NpStr& BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::UnknownValue = NoneStr;
template
const NpStr& BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::UnknownValue;

template<>
const int BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::DefaultValue = IoPinEdgeBlockingFile::NoEdgeTrigger;
template
const int BinaryStringSerDes<IoPinEdgeBlockingFile::EdgeTriggerType>::DefaultValue;

IoPinEdgeBlockingFile::IoPinEdgeBlockingFile(const char* filename, Npfs::Directory* parent, IoPin& variable, uint16_t umask)
: IoPinFile(filename, parent, variable, umask),
  edgeCount(0),
  fileReadWillBlock(0),
  lastInputState(variable.isActive()),
  blockForRisingEdge(false),
  blockForFallingEdge(false)
{
  EdgeTriggerType trg{NoEdgeTrigger};
  setEdgeMode(trg);
}

//IoPinEdgeBlockingFile::~IoPinEdgeBlockingFile()
//{
//  // Auto-generated destructor stub
//}

IoPinEdgeBlockingFile::IoBufferForBlocking::IoBufferForBlocking(IoBufferForBlocking * &anchor)
: hadTriggerEdge(false), triggerState(false),
  prev(nullptr), next(nullptr)
{
  if (!anchor)
    prev = &anchor;
  else
  {
    IoBufferForBlocking * c = anchor;

    while (c->next)
      c = c->next;

    prev = &c->next;
  }
  *prev = this;
}

IoPinEdgeBlockingFile::IoBufferForBlocking::~IoBufferForBlocking()
{
  assert(prev);

  if (prev)
    *prev = next;

  prev = 0;
  next = 0;
}

IoPinFile::IoBuffer* IoPinEdgeBlockingFile::makeIoBuffer()
{
  assert(0 != mm);
  auto b = new(mm) IoBufferForBlocking(clients);

  return b;
}

void IoPinEdgeBlockingFile::inputChanged(bool isActive)
{
  lastInputState = isActive;

  if (fileReadWillBlock)
    fileReadWillBlock->send(false);

  bool hadTrigger = (blockForRisingEdge && isActive) || (blockForFallingEdge && !isActive);
  IoBufferForBlocking* c = clients;

  if (hadTrigger)
  {
    edgeCount++;
    while (c)
    {
      if (!c->hadTriggerEdge)
      {
        c->triggerState = isActive;
        c->hadTriggerEdge = true;
      }

      c = c->next;
    }
  }
}

bool IoPinEdgeBlockingFile::blockRead(IoBuffer& wRef_)
{
//  IoBufferForBlocking& workRef = static_cast<IoBufferForBlocking&>(wRef_);

  bool block = blockForFallingEdge || blockForRisingEdge;

  return block;
}

bool IoPinEdgeBlockingFile::releaseRead(IoBuffer& wRef_)
{
  IoBufferForBlocking& workRef = static_cast<IoBufferForBlocking&>(wRef_);
  bool release = !blockForFallingEdge && !blockForFallingEdge;

  if (!fileReadWillBlock)
  {
    bool isActive = myIoPin.isActive();
    if (lastInputState != isActive)
      inputChanged(isActive);
  }

  if (workRef.hadTriggerEdge)
  {
    release = true;
    if (fileReadWillBlock)
    {
      bool triggerPending = false;
      IoBufferForBlocking * c = clients;

      while (c)
      {
        triggerPending |= c->hadTriggerEdge;
        c = c->next;
      }

      if (!triggerPending)
        fileReadWillBlock->send(true);
    }
  }

  return release;
}

const char* IoPinEdgeBlockingFile::readValue(IoBuffer& wRef_)
{
  const char* inVal = "invalid read value";

  if (blockForFallingEdge || blockForRisingEdge)
  {
    IoBufferForBlocking& workRef = static_cast<IoBufferForBlocking&>(wRef_);

    inVal = workRef.triggerState ? string->activeString : string->passiveString;
  }
  else
    inVal = lastInputState ? string->activeString : string->passiveString;

  return inVal;
}

//bool IoPinEdgeBlockingFile::isWriteAccepted(IoBuffer& workRef, const unsigned char* buffer, unsigned bufLen, uint64_t offset)
//{
//  return false;
//}

void IoPinEdgeBlockingFile::setEdgeTriggerInHw(EdgeTriggerType t)
{
  // intentionally left empty
}

bool IoPinEdgeBlockingFile::getEdgeMode(EdgeTriggerType& trg)
{
  if (!blockForFallingEdge && !blockForRisingEdge)
    trg = NoEdgeTrigger;
  else if (blockForFallingEdge && !blockForRisingEdge)
    trg = FallingEdgeTrigger;
  else if (blockForRisingEdge && !blockForFallingEdge)
    trg = RisingEdgeTrigger;
  else if (blockForFallingEdge && blockForRisingEdge)
    trg = BothEdgeTigger;

  return true;
}

bool IoPinEdgeBlockingFile::setEdgeMode(EdgeTriggerType& trg)
{
  switch (trg)
  {
  case NoEdgeTrigger:
    blockForFallingEdge = false;
    blockForRisingEdge = false;
    break;
  case RisingEdgeTrigger:
    blockForFallingEdge = false;
    blockForRisingEdge = true;
    break;
  case FallingEdgeTrigger:
    blockForFallingEdge = true;
    blockForRisingEdge = false;
    break;
  case BothEdgeTigger:
    blockForFallingEdge = true;
    blockForRisingEdge = true;
    break;
  }

  setEdgeTriggerInHw(trg);

  return true;
}

bool IoPinEdgeBlockingFile::getEdgeCountClear(uint32_t& cnt)
{
  cnt = 0;

  edgeCount.compare_exchange_weak(cnt, 0);

  return true;
}


} /* namespace Npfs */
