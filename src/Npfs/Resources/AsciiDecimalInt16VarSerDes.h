/*
 * AsciiDecimalInt16VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALINT16VARSERDES_H_
#define ASCIIDECIMALINT16VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for int16_t scalar variables to and from
 * decimal ASCII representation.
 *
 * It uses internally the strtol() routine with number base 10, so all rules for this apply.
 */
class AsciiDecimalInt16VarSerDes
{
public:
  typedef int16_t VarType;
  typedef int16_t VarConstructionType;

  AsciiDecimalInt16VarSerDes(VarType& myVar);

  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 6;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  VarType& variable;
};

}

#endif /* ASCIIDECIMALINT16VARSERDES_H_ */
