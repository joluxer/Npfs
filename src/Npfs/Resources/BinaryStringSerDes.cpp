/*
 * BinaryStringSerDes.cpp
 *
 *  Created on: 31.08.2015
 *      Author: lode
 */

#include "BinaryStringSerDes.h"

#include <cstring>
#include <cctype>

namespace Npfs
{

//BinaryStringSerDesBase::BinaryStringSerDesBase()
//{
//  // Auto-generated constructor stub
//
//}
//
//BinaryStringSerDesBase::~BinaryStringSerDesBase()
//{
//  // Auto-generated destructor stub
//}

void BinaryStringSerDesBase::serializeFromInt(int val, const std::pair<int, const NpStr *const>* array, const NpStr& UnknownValue, unsigned char* buffer, unsigned bufferLength)
{
  while (array->second)
  {
    if (array->first == val)
      break;
    array++;
  }

  const NpStr* str = array->second;

  if (!str)
  {
    str = &UnknownValue;
  }

  unsigned count = str->len;
  if (count > (bufferLength - 1))
    count = bufferLength - 1;

  memcpy(buffer, str->str, count);
  buffer[count] = '\n';
  if (count < (bufferLength - 1))
  {
    memset(buffer + count + 1, ' ', bufferLength - count - 2);
    buffer[bufferLength - 1] = '\n';
  }
}

int BinaryStringSerDesBase::deserializeToInt(const std::pair<int, const NpStr *const>* array, int DefaultValue, const unsigned char* buffer, unsigned bufferLength)
{
  int retVal = DefaultValue;

  // trim leading white space
  while (bufferLength && isspace(*buffer))
  {
    buffer++;
    bufferLength--;
  }

  // trim trailing white space
  while (bufferLength && isspace(buffer[bufferLength - 1]))
    bufferLength--;

  if (bufferLength)
  {
    NpStr str;
    str.str = (const char*)buffer;
    str.len = bufferLength;

    while (array->second)
    {
      if (*array->second == str)
        break;
      array++;
    }

    if (array->second)
      retVal = array->first;
  }

  return retVal;
}


} /* namespace Npfs */
