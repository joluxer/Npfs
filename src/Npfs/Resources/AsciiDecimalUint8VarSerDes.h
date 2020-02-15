/*
 * AsciiDecimalUint8VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALUINT8VARSERDES_H_
#define ASCIIDECIMALUINT8VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint8_t scalar variables to and from
 * hexadecimal ASCII representation (0x12 form).
 *
 * It uses internally the strtoul() routine with number base 16, so all rules for this apply.
 */
class AsciiDecimalUint8VarSerDes
{
public:
  typedef uint8_t VarType;
  typedef uint8_t VarConstructionType;

  AsciiDecimalUint8VarSerDes(VarType& myVar);

  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 4;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  VarType& variable;
};

}

#endif /* ASCIIDECIMALUINT8VARSERDES_H_ */
