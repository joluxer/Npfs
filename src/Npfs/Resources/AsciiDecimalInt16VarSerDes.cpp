/*
 * AsciiDecimalInt16VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiDecimalInt16VarSerDes.h"

#include "Util/PrintfToStream.h"
#include "Util/StringBufferStream.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiDecimalInt16VarSerDes::AsciiDecimalInt16VarSerDes(VarType& myVar)
: variable(myVar)
{}

unsigned AsciiDecimalInt16VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  StringBufferStream stringBuffer(buffer, bufferLength);
  PrintfToStream fString(stringBuffer);

  auto n = fString.printf("%6d\n", variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalInt16VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  long i;
  char* tail;

  i = strtol((const char*) buffer, &tail, 10);

  if (tail > (char*)buffer)
  {
    variable = static_cast<VarType>(i);
    success = true;
  }

  return success;
}

}
