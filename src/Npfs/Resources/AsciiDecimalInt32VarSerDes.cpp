/*
 * AsciiDecimalInt32VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalInt32VarSerDes.h"

#include "Util/PrintfToStream.h"
#include "Util/StringBufferStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalInt32VarSerDes::AsciiDecimalInt32VarSerDes(VarType& myVar)
: variable(myVar)
{}

unsigned AsciiDecimalInt32VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  auto n = fString.printf("%11d\n", variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalInt32VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  char* tail;

  auto i = strtol((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = static_cast<VarType>(i);
    success = true;
  }

  return success;
}

}
