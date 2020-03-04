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
#include <array>

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
  template<int N, int M = N, char ...D>
  struct BufferInitHelper : BufferInitHelper<N - 1, M, D..., ' '> {};
  template<int M, char ...D>
  struct BufferInitHelper<0, M, D...>
  {
    static constexpr ::std::array<char, M> table = { D... };
  };

public:
  constexpr
  NpStrBuffer()
  : NpStrBufferPtr(buffer.data(), BufferSize), buffer(BufferInitHelper<BufferSize>::table)
  {}
  NpStrBuffer(const char* s)
  : NpStrBufferPtr(buffer.data(), BufferSize)
  {
    *this = s;
  }
  NpStrBuffer(const NpStr& s)
  : NpStrBufferPtr(buffer.data(), BufferSize)
  {
    *this = s;
  }
  NpStrBuffer(NpStrBufferPtr&& other)
  : NpStrBufferPtr(buffer.data(), BufferSize)
  {
    auto l = other.len;

    if (l > bufferSpace)
      l = bufferSpace;

    memcpy(str, other.str, l);
    len = l;

    other.len = 0;
    memset(other.str, ' ', other.bufferSpace);  // in NpStr Typen sind NUL Zeichen verboten, daher wird auch ein leerer Puffer mit Leerzeichen vorbelegt.
  }
  NpStrBuffer(const NpStrBuffer& s)
  : NpStrBufferPtr(buffer.data(), BufferSize)
  {
    *this = s;
  }
//  virtual
//  ~NpStrBuffer();

  using NpStrBufferPtr::operator=;
  using NpStrBufferPtr::operator==;
  using NpStrBufferPtr::operator!=;
protected:
  ::std::array<char, BufferSize> buffer;
};

} /* namespace Npfs */

#endif /* NPSTRBUFFER_H_ */
