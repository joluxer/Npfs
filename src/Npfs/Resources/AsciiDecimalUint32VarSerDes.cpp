/*
 * AsciiDecimalUint32VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalUint32VarSerDes.h"

#include "Util/PrintfToStream.h"
#include "Util/StringBufferStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalUint32VarSerDes::AsciiDecimalUint32VarSerDes(uint32_t& myVar)
: variable(myVar)
{}

unsigned AsciiDecimalUint32VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  auto n = fString.printf("%10u\n", variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalUint32VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  uint32_t ui;
  char* tail;

  ui = strtoul((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = ui;
    success = true;
  }

  return success;
}

}
