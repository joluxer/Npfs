/*
 * AdmUsersFile.cpp
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#include "AdmUsersFile.h"

#include <string.h>

namespace Npfs
{

AdmUsersEntry* AdmUsersFile::file = 0;
AdmUsersEntry* AdmUsersFile::nilUser = 0;
uint64_t AdmUsersFile::fileLength = 0;

AdmUsersFile::AdmUsersFile(Directory* parent)
: Resource("users", parent, Qtfile)
{
  stat.uid = "adm";
  stat.gid = "adm";
  stat.muid = "adm";
  stat.setMode(stat.mode & ~(0222)); // this file is read only, remove all write bits from the default

  stat.length = fileLength;
}

//AdmUsersFile::~AdmUsersFile()
//{
//  // Auto-generated destructor stub
//}

Resource::ResultMessage AdmUsersFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage err = OpSuccess;

  iounit = 0;

  return err;
}

Resource::ResultMessage AdmUsersFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)  // in count will be returned the actual read amount of data
{
  uint64_t pos = 0;
  AdmUsersEntry* entry = file;
  uint32_t bufferSize = count;

  // look for the first entry
  count = 0;
  while (entry && (count < bufferSize))
  {
    if (0 == entry->stringLength)
    {
      entry->calcStringLength();
    }

    if ((pos < offset) && ((pos + entry->stringLength) > offset))
    {
      // string begins before offset and ends inside copied range
      count += entry->printLine((char *)(dataBuffer + count), bufferSize - count, offset - pos);
    }
    else if (pos >= offset)
    {
      // string begins inside the copied range, may end outside, but is correctly limited
      count += entry->printLine((char *)(dataBuffer + count), bufferSize - count, 0);
    }
    pos += entry->stringLength;
    entry = entry->next;
  }

  stat.atime = getCurrentTime();

  return OpSuccess;
}

void AdmUsersFile::appendEntry(AdmUsersEntry& nUser)
{
  if (file)
  {
    file->appendToList(nUser);
  }
  else
  {
    file = &nUser;
  }

  AdmUsersEntry* e = file;
  uint64_t l = 0;

  while(e)
  {
    l += e->stringLength;
    e = e->next;
  }

  fileLength = l;

}

void AdmUsersFile::removeEntry(const AdmUsersEntry& user)
{
  AdmUsersEntry *e = file, *o = 0;

  while (e && (user != *e))
  {
    o = e;
    e = e->next;
  }

  if (user == *e)
  {
    if (o)
      o->next = e->next;
    else
      file = e->next;
  }

  uint64_t l = 0;
  e = file;

  while (e)
  {
    l += e->stringLength;
    e = e->next;
  }

  fileLength = l;

}

AdmUsersEntry* AdmUsersFile::getEntry(const NpUser& spUser)
{
  AdmUsersEntry* found = nilUser;
  AdmUsersEntry* e = file;

  while (e)
  {
    // check user name string and numeric id for equality
    if ((strcmp(e->name, spUser.uname) == 0))
    {
      found = e;
      break;
    }
    e = e->next;
  }

  return found;
}

AdmUsersEntry* AdmUsersFile::getEntry(const NpStr& uname)
{
  AdmUsersEntry* found = nilUser;
  AdmUsersEntry* e = file;

  while (e)
  {
    // check user name string for equality
    if (uname == NpStr(e->name))
    {
      found = e;
      break;
    }
    e = e->next;
  }

  return found;
}

AdmUsersEntry* AdmUsersFile::getEntry(const char* uname)
{
  return getEntry(NpStr(uname));
}

const NpUser* AdmUsersFile::getNpUser(const NpStr& uname)
{
  AdmUsersEntry* e = getEntry(uname);
  if (e)
  {
    return &(e->groupData);
  }
  else
  {
    return 0;
  }
}

const NpUser* AdmUsersFile::getNpUser(const char* uname)
{
  return getNpUser(NpStr(uname));
}

const NpStat& AdmUsersFile::getStat(Npfs::BlockingOpState* &opRef)
{
  stat.length = fileLength;

  return stat;
}

void AdmUsersFile::flush(Npfs::OpenIoState& workRef)
{
  // intentionally left empty, this class is stateless
}

void AdmUsersFile::catchAllUnknownAs(const char* uname)
{
  if (uname)
  {
    nilUser = getEntry(uname);
  }
  else
  {
    nilUser = 0;
  }
}

void AdmUsersFile::catchAllUnknownAs(AdmUsersEntry& uname)
{
  nilUser = &uname;
}

}
