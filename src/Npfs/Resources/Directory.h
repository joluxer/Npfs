/*
 * Directory.h
 *
 *  Created on: 28.05.2012
 *      Author: lode
 */

#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include "Resource.h"
#include "Callback.h"
#include "pt/pt.h"

namespace Npfs
{

/***
 * This class implements a simple Directory to be held in RAM.
 *
 * The Directory has a name, needs a parent Directory during construction
 * or needs to to be mounted at a parent Directory later.
 *
 * The Directory is the fundamental concept of presenting resources in a
 * hierarchical sorted manner. A Directory hold references to Resources,
 * which may be Directories again or file Resources of all kind.
 *
 * The Directory holds one reference to the first member of the chain of all its child
 * Resources, one reference to its siblings (appearing on the same directory level)
 * and a reference to its parent Directory.
 *
 * The class must be the base class for custom directory implementations, such as flash drives
 * and other mass storage media. Other Resources, returning isDir() == true cannot be
 * queried to create new Resources.
 *
 * As most Resource implementations needs the Directory as well a MemoryManager for dynamic
 * data during the runtime. The MemoryManager is a static connection, assuming, that all
 * dynamic runtime data may be obtained from the same memory pool.
 */
class Directory: public Npfs::Resource
{
public:
  Directory(const char* dirName, Directory* parent = 0, uint16_t umask = 0222);
  Directory(const NpStat& newStat, Directory* parent = 0);
//  virtual ~Directory();

  static void useMemory(MemoryManager* mm);

  virtual
  ResultMessage create(Npfs::Resource* &newRessource, Npfs::OpenIoState& workRef, const Npfs::NpStr& name, const Npfs::NpUser& user, uint32_t perm, uint8_t mode, uint32_t& iounit);
  virtual
  ResultMessage open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit) override;
  virtual
  ResultMessage read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset) override;  // in count will be returned the actual read amount of data
  virtual
  ResultMessage write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset) override;
  virtual
  void flush(Npfs::OpenIoState& workRef);  ///< cancels the referenced I/O operation immediately, the server forgets about the operation after return of this method

  virtual
  ResultMessage remove(Npfs::BlockingOpState* &opRef, const NpUser& user) override;

  virtual
  ResultMessage modifyStat(Npfs::BlockingOpState* &opRef, const Npfs::NpStat& wstat) override;

  virtual
  Resource* walkTo(Npfs::BlockingOpState* &opRef, const Npfs::NpStr& path) override;

  virtual
  bool isDir() const override;

  Directory& mount(Resource& file);  ///< this mounts the given resource inside the directory

  Directory& delegateRemoveTo(const Callback<Npfs::Resource::ResultMessage, Npfs::Directory&, Npfs::BlockingOpState* &>* handler);
  Directory& delegateEntryCreationTo(const Callback<Npfs::Resource::ResultMessage, Npfs::BlockingOpState*&, Npfs::Resource* &, const Npfs::NpStat&>* handler);
  Directory& onWstatSend(const Callback<void, Npfs::Directory&>* signal);

  static Resource *const WalkIsPending;

protected:
  Resource* child;
  const Callback<Npfs::Resource::ResultMessage, Npfs::Directory&, Npfs::BlockingOpState* &>* removeDelegation;
  const Callback<Npfs::Resource::ResultMessage, Npfs::BlockingOpState*&, Npfs::Resource* &, const Npfs::NpStat&>* createEntryDelegation;
  const Callback<void, Npfs::Directory&>* wstatSignal; // this is sent on successful modification of the dir.stat by a wstat fcall, i.e. for rename or gid change

  static MemoryManager* mm;

  struct DirIoRef: public IoState
  {
    uint64_t currentDirOffset;
    unsigned entryOffset;
    Resource* nextEntry;
  };

  struct EntryCreateOpState: public BlockingOpState
  {
    NpStat newStat;
    BlockingOpState* delegateOpState;
    struct pt engineState;
    ResultMessage result;
  };

  void calcCreationPermissions(NpStr& newGroup, uint32_t& newPerm);

  PT_THREAD(entryCreatorEngine(EntryCreateOpState* s, Resource* &newRessource, const NpStr& name, const NpUser& user, uint32_t perm));

  friend class Resource;
};

inline
Directory& Directory::delegateRemoveTo(const Callback<Npfs::Resource::ResultMessage, Npfs::Directory&, Npfs::BlockingOpState* &>* handler)
{
  removeDelegation = handler;
  return *this;
}

inline
Directory& Directory::delegateEntryCreationTo(const Callback<Npfs::Resource::ResultMessage, Npfs::BlockingOpState*&, Npfs::Resource* &, const Npfs::NpStat&>* handler)
{
  createEntryDelegation = handler;
  return *this;
}

inline
Directory& Directory::onWstatSend(const Callback<void, Npfs::Directory&>* signal)
{
  wstatSignal = signal;
  return *this;
}



inline
void Directory::useMemory(MemoryManager* mm_)
{
  mm = mm_;
}

}

#endif /* DIRECTORY_H_ */
