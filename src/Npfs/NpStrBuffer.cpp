/*
 * NpStrBuffer.cpp
 *
 *  Created on: 03.02.2020
 *      Author: lode
 */

#include "NpStrBuffer.h"

#include <utility>
#include <cassert>

namespace Npfs
{

NpStrBufferPtr::NpStrBufferPtr(char* s)
: NpStrBufferPtr()
{
  str = s;
  len = strlen(s);
  // bufferspace is left zero intentionally
}

NpStrBufferPtr::NpStrBufferPtr(NpStrBufferPtr&& s)
: NpStrBufferPtr()
{
  *this = ::std::move(s);
}

NpStrBufferPtr::NpStrBufferPtr(const NpStr& s)
: NpStrBufferPtr()
{
  str = const_cast<char*>(s.str);
  len = s.len;
  // bufferspace is left zero intentionally
}

NpStrBufferPtr::NpStrBufferPtr(const NpStrBufferPtr& s)
: len(s.len), str(s.str), bufferSpace(s.bufferSpace)
{}

//NpStrBuffer::~NpStrBuffer()
//{
//  // Auto-generated destructor stub
//}

NpStrBufferPtr& NpStrBufferPtr::operator=(const char* s)
{
  assert(str and (0 != bufferSpace) && "the assignement target must have writable buffer space");

  auto l = strlen(s);

  if (l > bufferSpace)
    l = bufferSpace;

  memcpy(str, s, l);
  len = l;

  return *this;
}

NpStrBufferPtr& NpStrBufferPtr::operator=(const NpStr& other)
{
  assert(str and (0 != bufferSpace) && "the assignement target must have writable buffer space");

  auto l = other.len;

  if (l > bufferSpace)
    l = bufferSpace;

  memcpy(str, other.str, l);
  len = l;

  return *this;
}

NpStrBufferPtr& NpStrBufferPtr::operator=(NpStrBufferPtr&& other)
{
  len = other.len;
  str = other.str;
  bufferSpace = other.bufferSpace;

  other.len = 0;
  other.str = nullptr;
  other.bufferSpace = 0;

  return *this;
}

NpStrBufferPtr& NpStrBufferPtr::operator=(const NpStrBufferPtr& other)
{
  assert(str and (0 != bufferSpace) && "the assignement target must have writable buffer space");

  auto l = other.len;

  if (l > bufferSpace)
    l = bufferSpace;

  memcpy(str, other.str, l);
  len = l;

  return *this;
}

bool NpStrBufferPtr::operator==(const NpStrBufferPtr& s) const
{
  if (&s == this)
    return true;

  if ((s.str == str) and (s.len == len))
    return true;

  if (s.len != len)
    return false;

  if (0 == memcmp(s.str, str, len))
    return true;

  return false;
}

bool NpStrBufferPtr::operator==(const NpStr& s) const
{
  if ((s.str == str) and (s.len == len))
    return true;

  if (s.len != len)
    return false;

  if (0 == memcmp(s.str, str, len))
    return true;

  return false;
}

bool NpStrBufferPtr::operator==(const char *s) const
{
  if (s == str)
    return true;

  if (0 == strncmp(s, str, len))
    return true;

  return false;
}

} /* namespace Npfs */
