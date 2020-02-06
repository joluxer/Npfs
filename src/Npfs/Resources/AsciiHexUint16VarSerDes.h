/*
 * AsciiHexUint16VarSerDes.h
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#ifndef ASCIIHEXUINT16VARSERDES_H_
#define ASCIIHEXUINT16VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint8_t scalar variables to and from
 * hexadecimal ASCII representation (0x12 form).
 *
 * It uses internally the strtoul() routine with number base 16, so all rules for this apply.
 */
class AsciiHexUint16VarSerDes
{
public:
  AsciiHexUint16VarSerDes(uint16_t& myVar);

  typedef uint16_t VarType;
  typedef uint16_t VarConstructionType;
  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes = 7;

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength);

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength);

protected:
  uint16_t& variable;
};

}

#endif /* ASCIIHEXUINT16VARSERDES_H_ */
