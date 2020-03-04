/*
 * Resource.h
 *
 *  Created on: 28.05.2012
 *      Author: lode
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "../NpStr.h"
#include "../NpQid.h"
#include "../NpStat.h"
#include "../NpUser.h"
#include "../NpfsConstants.h"
#include "BlockingOpState.h"
#include "OpenIoState.h"

extern "C" uint32_t getCurrentTime();

namespace Npfs
{

class Directory;


/***
 * This class represents the central concept of presenting C++ objects as file or directory
 * via the 9P2000 to the user of the 9P2000 file system.
 *
 * Every resource has a name, a parent directory and supports a set of file operations.
 *
 * Each of these operations returns a special value, if it was successful or if it has no
 * memory. Everything else is return as a string. There exists a predefined set of
 * return strings, that are common to be understood, but virtually every return value
 * is allowed, if it fits into the Rerror message.
 *
 * Each file, that contains data or, in case of device files, transports data, inherits from this
 * class.
 *
 * A resource implementation needs to implement only methods, which it will support. Unimplemented
 * methods default to reject the action safely.
 *
 * The Resource can have siblings, which chain up. All siblings in a chain appear on the same directory
 * level. Consequently a Directory holds only one reference to the first member of the chain.
 * Each Resource holds a reference to its parent Directory.
 *
 * Directories are special resource files, which are already implemented for the simple case of
 * virtual directories, held in RAM. @see Directory.
 *
 * Resources from external data storage (disk, RAM disk, Flash drive etc.) need other implementations,
 * fully based on Resource.
 */
class Resource
{
public:
  Resource(const char* filename, Directory* parent, uint8_t type = Qtfile, uint16_t umask = 0133);
  Resource(const NpStat& newStat, Directory* parent);
  virtual ~Resource();

  typedef const char* ResultMessage;
  static const ResultMessage OpSuccess;
  static const ResultMessage NoMemory;

  virtual
  ResultMessage open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit) =0;
  virtual
  ResultMessage read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset);  // in count will be returned the actual read amount of data, the default implementation refuses the action
  virtual
  ResultMessage write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset); // the default implementation refuses the action
  virtual
  void flush(Npfs::OpenIoState& workRef) = 0;  ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method, this call blocks, until flush completes, may be called multiple times for the same operation
  virtual
  ResultMessage close(Npfs::OpenIoState& workRef);  // the default implementation just deletes ioState and returns, will be called only, if workRef.ioState is not zero
  virtual
  ResultMessage remove(Npfs::BlockingOpState* &opRef, const Npfs::NpUser& user); // the resource is asked to remove itself after it is umounted from paren (after call to umountForRemove() ); the default implementation refuses the action
  virtual
  const NpStat& getStat();
  virtual
  ResultMessage modifyStat(Npfs::BlockingOpState* &opRef, const Npfs::NpStat& wstat); // the default implementation modifies stat according the rules from 9P
  virtual
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);   // the default implementation refuses truncations

  virtual
  Resource* walkTo(Npfs::BlockingOpState* &opRef, const Npfs::NpStr& path); ///< tries to walk into the directory, the default implementation returns Null, 'cause a file cannot walk to a lower entry, if blocking return ~0

  virtual
  bool isDir() const;

  Directory* getParent() const;

  bool isWritableBy(const NpUser& user) const;
  bool isReadableBy(const NpUser& user) const;
  bool isExecutableBy(const NpUser& user) const;
  bool isAppendOnly() const;
  bool isExclusiveOnly() const;

  Resource& chmod(uint16_t newMode);
  Resource& chown(const NpStr& newOwner);
  Resource& chgrp(const NpStr& newGrp);
  Resource& setAtime(uint32_t newAtime);
  Resource& setMtime(uint32_t newMtime, const NpStr& muser);
  Resource& setNewName(const char* nName);
  Resource& setNewSize(unsigned size);
  Resource& setExclusive(bool enable = true);
  Resource& setAppendOnly(bool aonly = true);

  const NpStr& name() const;
  const NpQid& qid() const;
  uint64_t size() const;

  bool isLocked() const;
  bool isLockedFor(void * lockId) const;
  bool lockFor(void * lockId);
  void unlock();  // avoid use of this, use unlockFrom() instead
  void unlockFrom(void * lockId);

  void incrAccessRef(); // used by Fid & Co, when holding references/pointers
  void decrAccessRef(); // used by Fid & Co, when holding references/pointers
  bool isAccessed() const;

  bool isMounted() const;
  void umount(); ///< this removes the ressource from the parent directory
  void umountForRemove(); ///< this removes the ressource from the parent directory while still referenced by some FIDs and, after the call to accessDropped(), the remove() method will be called

protected:
  Directory* parent;
  Resource* sibling;
  NpStat stat;

  virtual
  void accessDropped(); // this is called, when no more access references (FIDs) to the object are left, the default implementation drops the message

  static bool handleNoMemory(void * mem, ResultMessage& result);

  friend class Directory;

private:
  void * locked;
  unsigned accessRefCount;
};

inline
const NpStr& Resource::name() const
{
  return stat.name;
}

inline
const NpQid& Resource::qid() const
{
  return stat.qid;
}

inline
uint64_t Resource::size() const
{
  return stat.length;
}

inline
Directory* Resource::getParent() const
{
  return parent;
}

inline
bool Resource::isAppendOnly() const
{
  return (stat.mode & Dmappend);
}

inline
bool Resource::isExclusiveOnly() const
{
  return (stat.mode & Dmexcl);
}

inline
bool Resource::isLocked() const
{
  return locked;
}

inline
bool Resource::isLockedFor(void * lockId) const
{
  return (locked == lockId);
}

inline
bool Resource::isMounted() const
{
  return !!parent;
}

inline
bool Resource::lockFor(void * lockId)
{
  if (locked)
  {
    return false;
  }
  else
  {
    locked = lockId;
    return true;
  }
}

inline
void Resource::unlock()
{
  locked = 0;
}

inline
void Resource::unlockFrom(void * lockId)
{
  if (isLockedFor(lockId))
    locked = 0;
}

inline
Resource& Resource::setExclusive(bool enable)
{
  if (enable)
  {
    stat.setMode(stat.mode | Dmexcl);
  }
  else
  {
    stat.setMode(stat.mode & ~Dmexcl);
  }

  return *this;
}

inline
Resource& Resource::setAppendOnly(bool aonly)
{
  if (aonly)
  {
    stat.setMode(stat.mode | Dmappend);
  }
  else
  {
    stat.setMode(stat.mode & ~Dmappend);
  }

  return *this;
}

inline
void Resource::incrAccessRef()
{
  accessRefCount++;
}

inline
bool Resource::isAccessed() const
{
  return !!accessRefCount;
}

}

#endif /* RESOURCE_H_ */
