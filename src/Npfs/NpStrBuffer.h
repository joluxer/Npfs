/*
 * NpStrBuffer.h
 *
 *  Created on: 03.02.2020
 *      Author: lode
 */

#ifndef NPSTRBUFFER_H_
#define NPSTRBUFFER_H_

#include "NpStr.h"
#include "Callback.h"

#include <string.h>

namespace Npfs
{

class NpStrBufferPtr
{
public:
  constexpr
  NpStrBufferPtr()
  : len(0), str(nullptr), bufferSpace(0)
  {}
  NpStrBufferPtr(char* s);
  NpStrBufferPtr(const NpStr& s);
  NpStrBufferPtr(NpStrBufferPtr&& s);
  NpStrBufferPtr(const NpStrBufferPtr& s);
  constexpr
  NpStrBufferPtr(char* s, unsigned space, unsigned len = 0)
  : len(len), str(s), bufferSpace(space)
  {}
//  virtual
//  ~NpStrBufferPtr();


  unsigned len;
  char* str;
  unsigned bufferSpace; // set to zero, if str is actually not writable

  NpStrBufferPtr& operator=(const char* s);
  NpStrBufferPtr& operator=(const NpStr& s);
  NpStrBufferPtr& operator=(NpStrBufferPtr&& s);
  NpStrBufferPtr& operator=(const NpStrBufferPtr& s);

  bool operator==(const NpStrBufferPtr& s) const;
  bool operator==(const NpStr& s) const;
  bool operator==(const char *s) const;
  bool operator!=(const NpStrBufferPtr& s) const { return not (*this == s); }
  bool operator!=(const NpStr& s) const { return not (*this == s); }
  bool operator!=(const char *s) const { return not (*this == s); }
};

/*
 *
 */
template<unsigned BufferSize>
class NpStrBuffer: public NpStrBufferPtr
{
public:
  constexpr
  NpStrBuffer()
  : NpStrBufferPtr(buffer, BufferSize)
  {}
  NpStrBuffer(const char* s)
  : NpStrBufferPtr(buffer, BufferSize)
  {
    *this = s;
  }
  NpStrBuffer(const NpStr& s)
  : NpStrBufferPtr(buffer, BufferSize)
  {
    *this = s;
  }
  NpStrBuffer(NpStrBufferPtr&& other)
  : NpStrBufferPtr(buffer, BufferSize)
  {
    auto l = other.len;

    if (l > bufferSpace)
      l = bufferSpace;

    memcpy(str, other.str, l);
    len = l;

    other.len = 0;
    memset(other.str, 0, other.bufferSpace);
  }
  NpStrBuffer(const NpStrBuffer& s)
  : NpStrBufferPtr(buffer, BufferSize)
  {
    *this = s;
  }
//  virtual
//  ~NpStrBuffer();

  using NpStrBufferPtr::operator=;
  using NpStrBufferPtr::operator==;
  using NpStrBufferPtr::operator!=;
protected:
  char buffer[BufferSize];
};

} /* namespace Npfs */

#endif /* NPSTRBUFFER_H_ */
