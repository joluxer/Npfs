/*
 * BinaryUint16LEVarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "BinaryUint16LEVarSerDes.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

BinaryUint16LEVarSerDes::BinaryUint16LEVarSerDes(uint16_t& myVar)
: variable(myVar)
{}

unsigned BinaryUint16LEVarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  buffer[0] = variable >> 0;
  buffer[1] = variable >> 8;

  return 2;
}

bool BinaryUint16LEVarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = (bufferLength == DataLength_bytes);

  if (success)
  {
    variable = 0;
    variable |= buffer[0] << 0;
    variable |= buffer[1] << 8;
  }

  return success;
}

}
