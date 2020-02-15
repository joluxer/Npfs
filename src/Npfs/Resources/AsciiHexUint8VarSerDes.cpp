/*
 * AsciiHexUint8VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiHexUint8VarSerDes.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiHexUint8VarSerDes::AsciiHexUint8VarSerDes(uint8_t& myVar)
: variable(myVar)
{}

unsigned AsciiHexUint8VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);

  buffer[0] = '0';
  buffer[1] = 'x';

  buffer[2] = ((variable >> 4) & 0xf) + '0';
  if ('9' < buffer[2])
    buffer[2] += 'a' - '9' - 1;
  buffer[3] = ((variable >> 0) & 0xf) + '0';
  if ('9' < buffer[3])
    buffer[3] += 'a' - '9' - 1;
  buffer[4] = '\n';

  return 5;
}

bool AsciiHexUint8VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  uint32_t ui;
  char* tail;

  ui = strtoul((const char*) buffer, &tail, 16);

  if (tail > (char*)buffer)
  {
    variable = ui & 0xff;
    success = true;
  }

  return success;
}


}
