/*
 * Directory.cpp
 *
 *  Created on: 28.05.2012
 *      Author: lode
 */

#include "Directory.h"

#include "../NpfsConstants.h"
#include "../MsgBufferSerDes.h"

#include <cassert>
#include <cstring>

namespace Npfs
{

Resource *const Directory::WalkIsPending = reinterpret_cast<Resource*const>(~0);

MemoryManager* Directory::mm = 0;

Directory::Directory(const char* dirName, Directory* _parent, uint16_t umask)
: Resource(dirName ? dirName:"/", _parent, Qtdir, umask), child(0),
  removeDelegation(0), createEntryDelegation(0), wstatSignal(0)
{
  if (!parent)
  {
    // the root directory case
    parent = this;
  }
}

Directory::Directory(const NpStat& newStat, Directory* parent)
: Resource(NpStat{NpStat::ForceDirStat{}, newStat}, parent), child(0),
  removeDelegation(0), createEntryDelegation(0), wstatSignal(0)
{
  if (!parent)
  {
    // the root directory case
    parent = this;
  }
}

//Directory::~Directory()
//{
//  // Auto-generated destructor stub
//}

Directory& Directory::mount(Resource& file)
{
  if (0 == child)
  {
    child = &file;
  }
  else
  {
    Resource* r = child;

    while (r->sibling)
    {
      r = r->sibling;
    }

    r->sibling = &file;
  }

  file.parent = this;

  stat.qid.version++;

  return *this;
}

void Directory::flush(Npfs::OpenIoState& workRef)
{
  // intentionally left empty
}

PT_THREAD(Directory::entryCreatorEngine(EntryCreateOpState* myState, Resource* &newRessource, const NpStr& name, const NpUser& user, uint32_t perm))
{
  assert(myState);
  struct pt *pt = &myState->engineState;
  auto& result = myState->result;

  PT_BEGIN(pt);

  if (this->isWritableBy(user))
  {
    // wir müssen den Namen puffern, weil das name-Objekt (und sein Speicher) nur bis zum Ende des Fcalls lebt
    if (!myState->newStat.name.str)
    {
      NpStr nameBuf;

      nameBuf.len = name.len;
      nameBuf.str = (char*) npfsNew(nameBuf.len, mm);
      if (nameBuf.str)
      {
        memcpy(const_cast<void*>(static_cast<const void*>(nameBuf.str)), name.str, nameBuf.len);

        // set the correct permissions
        NpStr newGid;
        calcCreationPermissions(newGid, perm);

        // stat abspeichern
        myState->newStat = NpStat(NpStat::CreateEntry{}, perm, nameBuf, user, newGid);
      }
      else
      {
        result = NoMemory;
        PT_EXIT(pt);
      }
    }

    // nun die Co-Routine aufrufen
    PT_WAIT_WHILE(pt, !(result = createEntryDelegation->execute(myState->delegateOpState, newRessource, myState->newStat)));

    if (newRessource)
    {
      assert(result == OpSuccess && "es muss gleichzeitg mit der neuen Resource auch OpSuccess zurückgegeben werden");
    }
    else
    {
      PT_EXIT(pt);
    }

    if (result == OpSuccess)
    {
      assert(newRessource && "es muss gleichzeitg mit OpSuccess auch eine neue Resource zurückgegeben werden");
      this->mount(*newRessource);
    }
  }
  else
  {
    result = ErrorString::Eperm;
    PT_EXIT(pt);
  }

  PT_END(pt);

  return 0;
}

Resource::ResultMessage Directory::create(Resource* &newRessource, Npfs::OpenIoState& workRef, const NpStr& name, const NpUser& user, uint32_t perm, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = ErrorString::Enotimpl;

  if (createEntryDelegation)
  {
    if (!newRessource)
    {
      EntryCreateOpState* myState = static_cast<EntryCreateOpState*>(workRef.opState);

      if (!myState)
      {
        myState = new(mm) EntryCreateOpState;
        workRef.opState = myState;

        if (!myState)
          result = NoMemory;
        else
        {
          myState->result = 0;
          PT_INIT(&myState->engineState);
        }
      }

      if (myState)
      {
        entryCreatorEngine(myState, newRessource, name, user, perm);
        result = myState->result;

        if (result)
        {
          delete myState;
          workRef.opState = 0;
        }
      }
    }

    if (newRessource)
    {
      // nun die Co-Routine aufrufen
      result = newRessource->open(workRef, mode, iounit);
    }
  }

  return result;
}

Resource::ResultMessage Directory::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  NpQid qid;
  ResultMessage err = OpSuccess;
  DirIoRef* ioRef;

  if ((mode & Orexec) || (mode & Otrunc) || (mode & Orclose))
  {
    err = ErrorString::Eperm;
  }
  else
  {
    if (mode != Oread)
    {
      err = ErrorString::Eperm;
    }
    else
    {
      assert (0 != mm);
      ioRef = new(mm) DirIoRef;
      ioRef->nextEntry = this->child;
      ioRef->currentDirOffset = 0;
      ioRef->entryOffset = 0;
      workRef.ioState = ioRef;
    }
  }

  return err;
}

Resource::ResultMessage Directory::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)  // in count will be returned the actual read amount of data
{
  DirIoRef* ioRef = static_cast<DirIoRef*>(workRef.ioState);
  unsigned n;
  ResultMessage err = OpSuccess;

  // check for offset reset
  if (offset == 0)
  {
    ioRef->nextEntry = this->child;
    ioRef->currentDirOffset = 0;
  }

  if (offset == ioRef->currentDirOffset)
  {
    n = 0;
    unsigned statSz;
    while ((0 != ioRef->nextEntry) && (n < count))
    {
      statSz = ioRef->nextEntry->stat.statSize() + 2;
      if (statSz <= (count - n))
      {
        MsgBufferSerDes buffer(0, dataBuffer + n, count - n);

        buffer.put(ioRef->nextEntry->stat);
        n += statSz;
        ioRef->nextEntry = ioRef->nextEntry->sibling;
      }
      else
      {
        break;
      }
    }

    // return actual amount of data
    ioRef->currentDirOffset += n;
    count = n;
    stat.atime = getCurrentTime();
  }
  else
  {
    err = ErrorString::Ebadoffset;
  }

  return err;
}

Resource::ResultMessage Directory::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  return "Directory writes are not allowed";
}

bool Directory::isDir() const
{
  return true;
}

Resource* Directory::walkTo(Npfs::BlockingOpState* &opRef, const NpStr& path)
{
  Resource* target = child;

  while (target)
  {
    if (target->stat.name == path)
    {
      break;
    }
    target = target->sibling;
  }

  stat.atime = getCurrentTime();
  return target;
}

Resource::ResultMessage Directory::modifyStat(Npfs::BlockingOpState* &opRef, const NpStat& wstat)
{
  ResultMessage err = OpSuccess;

  if ((wstat.length != 0) && (~wstat.length != 0))
  {
    err = "Directory resizing not allowed";
  }
  else
  {
    err = Resource::modifyStat(opRef, wstat);
    if (wstatSignal && (OpSuccess == err))
      wstatSignal->send(*this);
  }

  return err;
}

Resource::ResultMessage Directory::remove(Npfs::BlockingOpState* &opRef, const NpUser& user)
{
  ResultMessage result = ErrorString::Enotimpl;

  if (removeDelegation)
  {
    result = removeDelegation->execute(*this, opRef);

    if (OpSuccess == result)
      npfsDelete(const_cast<void*>(static_cast<const void*>(stat.name.str)));
  }

  return result;
}

void Directory::calcCreationPermissions(NpStr& newGroup, uint32_t& newMode)
{
  newGroup = this->stat.gid;
  if (Dmdir & newMode)
  {
    // spec says for new dirs: perm & (~8r777 | (dir.perm & 8r777))
    newMode &= ~uint32_t(0777UL) | (stat.mode & uint32_t(0777UL));
  }
  else
  {
    // spec says for new files: perm & (~8r666 | (dir.perm & 8r666))
    newMode &= ~uint32_t(0666UL) | (stat.mode & uint32_t(0666UL));
  }
}

}
