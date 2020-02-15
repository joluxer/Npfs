/*
 * BinaryUint16LEVarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef BINARYUINT16LEVARSERDES_H_
#define BINARYUINT16LEVARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint16_t scalar variables to and from
 * binary little endian representation.
 */
class BinaryUint16LEVarSerDes
{
public:
  BinaryUint16LEVarSerDes(uint16_t& myVar);

  typedef uint16_t VarType;
  typedef uint16_t VarConstructionType;
  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 2;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  uint16_t& variable;
};

}

#endif /* BINARYUINT16LEVARSERDES_H_ */
