/*
 * AsciiDecimalInt32VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALINT32VARSERDES_H_
#define ASCIIDECIMALINT32VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for int16_t scalar variables to and from
 * decimal ASCII representation.
 *
 * It uses internally the strtol() routine with number base 10, so all rules for this apply.
 */
class AsciiDecimalInt32VarSerDes
{
public:
  typedef int32_t VarType;
  typedef int32_t VarConstructionType;

  AsciiDecimalInt32VarSerDes(VarType& myVar);

  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 8;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  VarType& variable;
};

}

#endif /* ASCIIDECIMALINT32VARSERDES_H_ */
