/*
 * AsciiDecimalUint32VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIDECIMALUINT32VARSERDES_H_
#define ASCIIDECIMALUINT32VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint32_t scalar variables to and from
 * decimal ASCII representation.
 *
 * It uses internally the strtoul() routine with number base 10, so all rules for this apply.
 */
class AsciiDecimalUint32VarSerDes
{
public:
  AsciiDecimalUint32VarSerDes(uint32_t& myVar);

  typedef uint32_t VarType;
  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 11;

  void serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  uint32_t& variable;
};

}

#endif /* ASCIIDECIMALUINT32VARSERDES_H_ */
