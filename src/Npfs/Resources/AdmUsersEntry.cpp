/*
 * AdmUsersEntry.cpp
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#include "AdmUsersEntry.h"

#include <string.h>

namespace Npfs
{

AdmUsersEntry::AdmUsersEntry(const char* _name, AdmUsersEntry* previousEntry, AdmUsersEntry* groupLeader)
: name(_name),
  leader(groupLeader == (AdmUsersEntry*)~0 ? this : groupLeader),
  member(0), stringLength(0), next(0)
{
  groupData.uname = name;
//  groupData.next = 0;

  if (previousEntry)
  {
    previousEntry->next = this;
    previousEntry->groupData.next = &(this->groupData);
  }

  calcStringLength();
}

AdmUsersEntry::~AdmUsersEntry()
{
  AdmGroupMember *c = member, *next;

  while (c)
  {
    next = c->next;
    delete c;
    c = next;
  }
}

unsigned AdmUsersEntry::calcStringLength()
{
  unsigned strLen;

  strLen = 3; // 3 colons

  // calc the id length
  strLen += strlen(name);
  // read the string lengths
  strLen += strlen(name);
  if (leader)
    strLen += strlen(leader->name);

  AdmGroupMember* m = member;
  while (m)
  {
    strLen += strlen(m->user->name);
    m = m->next;
    if (m)
      strLen += 1;  // behind the member is a comma, as we have more members
  }
  strLen++; // every line ends with a NewLine character

  stringLength = strLen;

  return strLen;
}

void AdmUsersEntry::copyString(const char* src, char* dest, unsigned destSize, unsigned& destFill, unsigned& srcOffset)
{
  unsigned srcLen = strlen(src);
  unsigned copyLen = srcLen;

  // do copy only, if offset is inside the src string
  if (srcOffset < srcLen)
  {
    unsigned copyLimit = destSize - destFill;
    char* destPos = dest + destFill;
    const char* srcStart = src + srcOffset;

    copyLen = srcLen - srcOffset;

    if (copyLimit < copyLen)
    {
      copyLen = copyLimit;
    }
    strncpy(destPos, srcStart, copyLen);

    destFill += copyLen;
  }

  // adjust offset by consumed string length
  if (srcOffset >= copyLen)
  {
    srcOffset -= copyLen;
  }
  else
  {
    srcOffset = 0;
  }
}

unsigned AdmUsersEntry::printLine(char * buffer, uint32_t bufSize, unsigned strOffset)
{
  unsigned bufFill = 0;

  // copy the id string
  copyString(name, buffer, bufSize, bufFill, strOffset);
  copyString(":", buffer, bufSize, bufFill, strOffset);
  copyString(name, buffer, bufSize, bufFill, strOffset);
  copyString(":", buffer, bufSize, bufFill, strOffset);
  if (leader)
  {
    copyString(leader->name, buffer, bufSize, bufFill, strOffset);
  }
  copyString(":", buffer, bufSize, bufFill, strOffset);
  {
    AdmGroupMember* m = member;
    if (m)  // first will be appended without leading comma
    {
      copyString(m->user->name, buffer, bufSize, bufFill, strOffset);
      m = m->next;
      while (m)
      {
        copyString(",", buffer, bufSize, bufFill, strOffset);
        copyString(m->user->name, buffer, bufSize, bufFill, strOffset);
        m = m->next;
      }
    }
  }
  copyString("\n", buffer, bufSize, bufFill, strOffset);

  return bufFill;
}

AdmUsersEntry& AdmUsersEntry::appendToList(AdmUsersEntry& nUser)  ///< this appends the given object to the chain of entries
{
  if (0 == nUser.stringLength)
  {
    nUser.calcStringLength();
  }

  AdmUsersEntry* c = this;
  while (c->next)
  {
    c = c->next;
  }

  c->next = &nUser;

  return nUser;
}

AdmUsersEntry& AdmUsersEntry::addMember(AdmUsersEntry& user)   ///< this appends a new group member
{
  AdmGroupMember* m = new AdmGroupMember(user);

  if (0 == member)
  {
    member = m;
  }
  else
  {
    AdmGroupMember* c = member;

    while (c->next)
    {
      c = c->next;
    }

    c->next = m;
  }

  return *this;
}

bool AdmUsersEntry::operator ==(const AdmUsersEntry& other) const
{
  bool equal;

  equal = (0 == strcmp(this->name, other.name));

  return equal;
}

bool AdmUsersEntry::operator !=(const AdmUsersEntry& other) const
{
  return ! this->operator ==(other);
}

}
