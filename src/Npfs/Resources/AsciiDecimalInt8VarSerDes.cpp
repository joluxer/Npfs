/*
 * AsciiDecimalInt8VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalInt8VarSerDes.h"

#include "Util/StringBufferStream.h"
#include "Util/PrintfToStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalInt8VarSerDes::AsciiDecimalInt8VarSerDes(VarType& myVar)
: variable(myVar)
{}

void AsciiDecimalInt8VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  fString.printf("%4d\n", variable);
}

bool AsciiDecimalInt8VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  int32_t i;
  char* tail;

  i = strtol((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = i & 0xff;
    success = true;
  }

  return success;
}


}
