/*
 * AsciiHexUint32VarSerDes.h
 *
 *  Created on: 16.08.2012
 *      Author: jlode
 */

#ifndef ASCIIHEXUINT32VARSERDES_H_
#define ASCIIHEXUINT32VARSERDES_H_

#include <stdint.h>

namespace Npfs
{

/***
 * This class is the serializer and deserializer for uint32_t scalar variables to and from
 * hexadecimal ASCII representation (0x12345678 form).
 *
 * It uses internally the strtoul() routine with number base 16, so all rules for this apply.
 */
class AsciiHexUint32VarSerDes
{
public:
  AsciiHexUint32VarSerDes(uint32_t &myVar);

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
#endif /* ASCIIHEXUINT32VARSERDES_H_ */
