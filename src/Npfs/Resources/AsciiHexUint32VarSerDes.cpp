/*
 * AsciiHexUint32VarSerDes.cpp
 *
 *  Created on: 16.08.2012
 *      Author: jlode
 */

#include "AsciiHexUint32VarSerDes.h"

#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

namespace Npfs
{

AsciiHexUint32VarSerDes::AsciiHexUint32VarSerDes(uint32_t &myVar)
: variable(myVar)
{}

unsigned AsciiHexUint32VarSerDes::serializeTo(unsigned char* buffer, unsigned bufferLength)
{
  unsigned i;
  unsigned shift;

  buffer[0] = '0';
  buffer[1] = 'x';

  assert(bufferLength >= DataLength_bytes);

  for (i = 2, shift = 24; i < (DataLength_bytes - 1); i+=2, shift -= 8)
  {
    buffer[i] = ((variable >> (shift + 4)) & 0xf) + '0';
    if ('9' < buffer[i])
      buffer[i] += 'a' - '9' - 1;
    buffer[i + 1] = ((variable >> shift) & 0xf) + '0';
    if ('9' < buffer[i + 1])
      buffer[i + 1] += 'a' - '9' - 1;
  }
  buffer[i] = '\n';

  return ++i;
}

bool AsciiHexUint32VarSerDes::deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
{
  bool success = false;
  uint32_t ui;
  char *tail;
  const char* cBuffer = (const char*) buffer;
  static_cast<void>(buffer);

  while (bufferLength && isspace(cBuffer[0]))
  {
    ++cBuffer;
    --bufferLength;
  }

  if (bufferLength)
  {
    ui = strtoul(cBuffer, &tail, 16);

    if (tail > cBuffer)
    {
      variable = ui;
      success = true;
    }
  }

  return success;
}

}
