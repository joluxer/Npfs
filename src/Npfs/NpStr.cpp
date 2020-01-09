/*
 * NpCstr.cpp
 *
 *  Created on: 20.07.2012
 *      Author: jlode
 */

#include "NpStr.h"

#include <string.h>

namespace Npfs
{

NpStr::NpStr()
: len(0), str(0)
{}

NpStr::NpStr(const char* s)
: len(s ? strlen(s) : 0), str(s)
{}

//NpCstr::~NpCstr()
//{
//  // Auto-generated destructor stub
//}

NpStr& NpStr::operator =(const char* s)
{
  str = s;
  len = strlen(s);

  return *this;
}

bool NpStr::operator ==(const NpStr& s) const
{
  bool equal = true;

  equal &= (s.len == len);
  if (equal)
    equal &= (0 == memcmp(s.str, str, len));

  return equal;
}

}
