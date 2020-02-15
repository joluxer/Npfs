/*
 * AsciiHexUint16VarSerDes.cpp
 *
 *  Created on: 29.09.2012
 *      Author: lode
 */

#include "AsciiHexUint16VarSerDes.h"

#include <assert.h>
#include <stdlib.h>

namespace Npfs
{

AsciiHexUint16VarSerDes::AsciiHexUint16VarSerDes(uint16_t& myVar)
: variable(myVar)
{}

unsigned AsciiHexUint16VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  assert(bufferLength >= DataLength_bytes);
  unsigned i;

  buffer[i = 0] = '0';
  buffer[++i] = 'x';

  buffer[++i] = ((variable >> 12) & 0xf) + '0';
  if (buffer[i] > '9')
    buffer[i] += 'A' - '9' - 1;
  buffer[++i] = ((variable >>  8) & 0xf) + '0';
  if (buffer[i] > '9')
    buffer[i] += 'A' - '9' - 1;
  buffer[++i] = ((variable >>  4) & 0xf) + '0';
  if (buffer[i] > '9')
    buffer[i] += 'A' - '9' - 1;
  buffer[++i] = ((variable >>  0) & 0xf) + '0';
  if (buffer[i] > '9')
    buffer[i] += 'A' - '9' - 1;

  buffer[++i] = '\n';

  return ++i;
}

bool AsciiHexUint16VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  uint32_t ui;
  char* tail;

  ui = strtoul((const char*) buffer, &tail, 16);

  if (tail > (char*)buffer)
  {
    variable = ui & 0xffff;
    success = true;
  }

  return success;
}


}
