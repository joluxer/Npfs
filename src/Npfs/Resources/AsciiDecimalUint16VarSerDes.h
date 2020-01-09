/*
 * AsciiDecimalUint16VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALUINT16VARSERDES_H_
#define ASCIIDECIMALUINT16VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint16_t scalar variables to and from
 * decimal ASCII representation.
 *
 * It uses internally the strtoul() routine with number base 10, so all rules for this apply.
 */
class AsciiDecimalUint16VarSerDes
{
public:
  AsciiDecimalUint16VarSerDes(uint16_t& myVar);

  typedef uint16_t VarType;
  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 6;

  void serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  uint16_t& variable;
};

}

#endif /* ASCIIDECIMALUINT16VARSERDES_H_ */
