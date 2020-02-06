/*
 * AsciiDecimalUint16VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalUint16VarSerDes.h"

#include "Util/PrintfToStream.h"
#include "Util/StringBufferStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalUint16VarSerDes::AsciiDecimalUint16VarSerDes(uint16_t& myVar)
: variable(myVar)
{}

unsigned AsciiDecimalUint16VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  auto n = fString.printf("%5u\n", variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalUint16VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  uint32_t ui;
  char* tail;

  ui = strtoul((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = ui & 0xffff;
    success = true;
  }

  return success;
}

}
