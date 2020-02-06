/*
 * AsciiDecimalFloat32LibcSerdes.cpp
 *
 *  Created on: 23.08.2017
 *      Author: lode
 */

#include "AsciiDecimalFloat32LibcSerdes.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace Npfs
{

AsciiDecimalFloat32LibcSerdes::AsciiDecimalFloat32LibcSerdes(VarType& myVar)
: variable(myVar)
{}

//AsciiDecimalFloat32LibcSerdes::~AsciiDecimalFloat32LibcSerdes()
//{
//  // Auto-generated destructor stub
//}

unsigned AsciiDecimalFloat32LibcSerdes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  auto n = snprintf(reinterpret_cast<char*>(buffer), bufferLength, "%*.*g\n", int(DataLength_bytes - 1), int(DataLength_bytes - 6), variable);

  if (n > int(bufferLength))
    n = bufferLength;

  return n;
}

bool AsciiDecimalFloat32LibcSerdes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  char* tail;

  auto result = strtof((const char*) buffer, &tail);

  if (tail > (char*)buffer)
  {
    variable = static_cast<VarType>(result);
    success = true;
  }

  return success;
}

} /* namespace Npfs */
