/*
 * Resource.cpp
 *
 *  Created on: 28.05.2012
 *      Author: lode
 */

#include "Resource.h"

#include "AdmGroup.h"
#include "../NpfsConstants.h"

#include "Directory.h"

#include <string.h>
#include <assert.h>

namespace Npfs
{

const Resource::ResultMessage Resource::OpSuccess = "operation successful";
const Resource::ResultMessage Resource::NoMemory = "Cannot allocate memory";

Resource::Resource(const char* name, Directory* _parent, uint8_t type, uint16_t umask)
: parent(_parent), sibling(0), locked(0), accessRefCount(0)
{
  assert(0 != name);

  stat.name = name;

  stat.qid.type = type;
  stat.qid.version = 0;
  stat.qid.path = reinterpret_cast<uint64_t>(this);

  stat.atime = stat.mtime = getCurrentTime();
  stat.gid = "root";
  stat.uid = "root";
  stat.muid = "root";

  stat.length = 0;
  stat.setMode((0777 & ~(uint32_t(umask))) | ((uint32_t)type << 24));
  stat.size = stat.statSize();

  if (parent)
  {
    parent->mount(*this);
  }
}

Resource::Resource(const NpStat& newStat, Directory* parent_)
: parent(parent_), sibling(0), stat(newStat), locked(0), accessRefCount(0)
{
  assert(0 != stat.name.str);

  if (parent)
  {
    parent->mount(*this);
  }
}

Resource::~Resource()
{
  umount();
}

void Resource::accessDropped()
{
  // intentionally left empty
}

void Resource::decrAccessRef()
{
  assert(!!accessRefCount);
  accessRefCount--;
  if (!accessRefCount)
    accessDropped();
}

Resource::ResultMessage Resource::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)  // in count will be returned the actual read amount of data
{
  count = 0;
  return ErrorString::Enotimpl;
}

Resource::ResultMessage Resource::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  count = 0;
  return ErrorString::Enotimpl;
}

Resource::ResultMessage Resource::close(Npfs::OpenIoState& workRef)
{
  if (workRef.ioState)
  {
    delete workRef.ioState;
    workRef.ioState = 0;
  }

  return OpSuccess;
}

const NpStat& Resource::getStat()
{
  return stat;
}

Resource::ResultMessage Resource::modifyStat(Npfs::BlockingOpState* &_opRef, const NpStat& wstat)
{
  ResultMessage err = OpSuccess;

  // change size
  if (wstat.length != ~((uint64_t)0))
  {
    err = trunc(_opRef, wstat.length);

    if (OpSuccess == err)
    {
      NpStat newStat = stat;

      // change name
      if (wstat.name.len != 0)
      {
        // FIXME: verlorener Speicher?
        newStat.name = wstat.name;
      }

      // chmod
      if (wstat.mode != (uint32_t)~0)
      {
        newStat.setMode((wstat.mode & ~Dmdir) | (stat.mode & Dmdir));
      }

      // change gid
      if (wstat.gid.len != 0)
      {
        // FIXME: verlorener Speicher?
        newStat.gid = wstat.gid;
      }

      stat = newStat;

      if (parent)
      {
        parent->stat.qid.version++;
      }
    }
  }

  return err;
}

bool Resource::isDir() const
{
  return ((stat.qid.type & Qtdir) != 0);
}

bool Resource::isWritableBy(const NpUser& user) const
{
  bool allowed = false;

  do // this is just a breakable block to avoid goto
  {
    if (stat.mode & 0002)
    {
      allowed = true;
      break;
    }

    if (AdmGroup(stat.gid).hasMember(user))
    {
      if (stat.mode & 0020)
      {
        allowed = true;
        break;
      }
    }

    if (user == stat.uid)
    {
      if (stat.mode & 0200)
      {
        allowed = true;
        break;
      }
    }
  } while (0);

  return allowed;
}

bool Resource::isReadableBy(const NpUser& user) const
{
  bool allowed = false;

  do // this is just a breakable block to acoid goto
  {
    if (stat.mode & 0004)
    {
      allowed = true;
      break;
    }

    if (AdmGroup(stat.gid).hasMember(user))
    {
      if (stat.mode & 0040)
      {
        allowed = true;
        break;
      }
    }

    if (user == stat.uid)
    {
      if (stat.mode & 0400)
      {
        allowed = true;
        break;
      }
    }
  } while (0);

  return allowed;
}

bool Resource::isExecutableBy(const NpUser& user) const
{
  bool allowed = false;

  do // this is just a breakable block to acoid goto
  {
    if (stat.mode & 0001)
    {
      allowed = true;
      break;
    }

    if (AdmGroup(stat.gid).hasMember(user))
    {
      if (stat.mode & 0010)
      {
        allowed = true;
        break;
      }
    }

    if (user == stat.uid)
    {
      if (stat.mode & 0100)
      {
        allowed = true;
        break;
      }
    }
  } while (0);

  return allowed;
}

Resource::ResultMessage Resource::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength)   // the default implementation refuses truncations
{
  return ErrorString::Eperm;
}

Resource::ResultMessage Resource::remove(Npfs::BlockingOpState* &opRef, const NpUser& user)
{
  // an implementation must do npfsDelete(stat.name.str);

  return ErrorString::Enotimpl;
}

Resource* Resource::walkTo(Npfs::BlockingOpState* &opRef, const NpStr& path)
{
  return 0;
}

Resource& Resource::chmod(uint16_t newMode)
{
  if (parent)
    parent->stat.atime = parent->stat.mtime = getCurrentTime();
  stat.setMode((stat.mode & ~0777) | (newMode & 0777));

  return *this;
}

Resource& Resource::chown(const NpStr& newOwner)
{
  if (parent)
    parent->stat.atime = parent->stat.mtime = getCurrentTime();
  // FIXME: verlorener Speicher?
  stat.uid = newOwner;
  stat.updateStatSize();

  return *this;
}

Resource& Resource::chgrp(const NpStr& newGrp)
{
  if (parent)
    parent->stat.atime = parent->stat.mtime = getCurrentTime();
  // FIXME: verlorener Speicher?
  stat.gid = newGrp;
  stat.updateStatSize();

  return *this;
}

Resource& Resource::setAtime(uint32_t newAtime)
{
  stat.atime = newAtime;

  return *this;
}

Resource& Resource::setMtime(uint32_t newMtime, const NpStr& muser)
{
  stat.atime = stat.mtime = newMtime;
  // FIXME: verlorener Speicher?
  stat.muid = muser;
  stat.updateStatSize();

  return *this;
}

Resource& Resource::setNewName(const char* nName)
{
  // FIXME: verlorener Speicher?
  stat.name = nName;
  stat.updateStatSize();

  return *this;
}

Resource& Resource::setNewSize(unsigned size)
{
  stat.length = size;

  return *this;
}

void Resource::umountForRemove()
{
  // check for the root entry
  if (parent)
  {
    // search entry in parents children
    Resource *r = parent->child, *leftSibling = 0;

    while (r && (this != r))
    {
      leftSibling = r;
      r = r->sibling;
    }
    if (this == r)
    {
      // remove entry from the parent
      if (leftSibling)
      {
        leftSibling->sibling = sibling;
      }
      else
      {
        parent->child = sibling;
      }
      parent = 0;
      sibling = 0;
    }
  }
}

void Resource::umount()
{
  assert(!isAccessed()); // this piece must not be in use any more

  umountForRemove();
}

bool Resource::handleNoMemory(void * mem, ResultMessage& result)
{
  if (mem)
  {
    return false;
  }

  result = NoMemory;

  return true;
}

}
