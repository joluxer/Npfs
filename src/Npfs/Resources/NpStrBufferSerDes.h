/*
 * NpStrBufferPtrSerDes.h
 *
 *  Created on: 03.02.2020
 *      Author: lode
 */

#ifndef NPSTRBUFFERSERDES_H_
#define NPSTRBUFFERSERDES_H_

#include "../NpStrBuffer.h"

#include <assert.h>
#include <string.h>

namespace Npfs
{

/**
 * A ser-des for TransactionFile and VariableFile.
 *
 * This serializer-deserializer manages a NpStrBufferPtr typed variable.
 * If the NpStrBufferPtr is empty (i.e. the str pointer of the object is zero),
 * the serialization and deserialization buffer is used as the str object.
 *
 */
template<unsigned StringLength, bool CanSerialize = true, bool CanDeserialize = true>
class NpStrBufferPtrSerDes
{
public:
  NpStrBufferPtrSerDes(NpStrBufferPtr& variable)
  : variable(variable), selfBuiltStr(false)
  {}

//  virtual
  ~NpStrBufferPtrSerDes();


  typedef NpStrBufferPtr VarType;
  typedef NpStrBuffer<StringLength> VarConstructionType;
  static const bool canSerialize = CanSerialize;
  static const bool canDeserialize = CanDeserialize;
  static const uint64_t DataLength_bytes = StringLength;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  NpStrBufferPtr& variable;
  bool selfBuiltStr:1; // set, if the variable.str is made from the serializer buffer, used to release the assignment in destructor
};

template<unsigned StringLength, bool CanSerialize, bool CanDeserialize>
NpStrBufferPtrSerDes<StringLength, CanSerialize, CanDeserialize>::~NpStrBufferPtrSerDes()
{
  if (selfBuiltStr)
  {
    variable.str = nullptr;
    variable.len = 0;
    variable.bufferSpace = 0;
  }
}

template<unsigned StringLength, bool CanSerialize, bool CanDeserialize>
unsigned NpStrBufferPtrSerDes<StringLength, CanSerialize, CanDeserialize>::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  if (variable.str)
  {
    auto count = variable.len;

    if (count > bufferLength)
      count = bufferLength;

    memcpy(buffer, variable.str, count);
  }
  else
  {
    // this makes the variable content temporary as long as the I/O operation lasts, but works safe for TransientFile and VariableFile
    selfBuiltStr = true;
    variable.str = reinterpret_cast<char*>(buffer);
    variable.len = bufferLength;
    variable.bufferSpace = DataLength_bytes;
  }

  return variable.len;
}

template<unsigned StringLength, bool CanSerialize, bool CanDeserialize>
bool NpStrBufferPtrSerDes<StringLength, CanSerialize, CanDeserialize>::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;

  if (variable.str)
  {
    auto count = bufferLength;

    if (count > variable.bufferSpace)
      count = variable.bufferSpace;

    memcpy(variable.str, buffer, count);
    variable.len = count;
    success = true;
  }
  else
  {
    // this makes the variable content temporary as long as the I/O operation lasts, but works safe for TransientFile and VariableFile
    selfBuiltStr = true;
    variable.str = const_cast<char*>(reinterpret_cast<const char*>(buffer));
    variable.len = bufferLength;
    variable.bufferSpace = DataLength_bytes;
    success = true;
  }

  return success;
}


} /* namespace Npfs */

#endif /* NPSTRBUFFERSERDES_H_ */
