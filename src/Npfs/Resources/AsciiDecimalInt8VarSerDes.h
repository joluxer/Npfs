/*
 * AsciiDecimalInt8VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALINT8VARSERDES_H_
#define ASCIIDECIMALINT8VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for int8_t scalar variables to and from
 * decimal ASCII representation.
 *
 * It uses internally the strtoul() routine with number base 10, so all rules for this apply.
 */
class AsciiDecimalInt8VarSerDes
{
public:
  typedef int8_t VarType;
  typedef int8_t VarConstructionType;

  AsciiDecimalInt8VarSerDes(VarType& myVar);

  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 5;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  VarType& variable;
};

}

#endif /* ASCIIDECIMALINT8VARSERDES_H_ */
