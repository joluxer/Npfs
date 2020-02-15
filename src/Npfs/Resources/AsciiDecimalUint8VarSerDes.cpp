/*
 * AsciiDecimalUint8VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalUint8VarSerDes.h"

#include "Util/StringBufferStream.h"
#include "Util/PrintfToStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalUint8VarSerDes::AsciiDecimalUint8VarSerDes(VarType& myVar)
: variable(myVar)
{}

unsigned AsciiDecimalUint8VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  auto n = fString.printf("%3u\n", variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalUint8VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  unsigned long i;
  char* tail;

  i = strtoul((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = i & 0xff;
    success = true;
  }

  return success;
}


}
