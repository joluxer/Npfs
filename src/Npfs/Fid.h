/*
 * Fid.h
 *
 *  Created on: 25.07.2012
 *      Author: jlode
 */

#ifndef FID_H_
#define FID_H_

#include "Resources/Resource.h"
#include "Resources/Directory.h"
#include "Resources/ResRefCntPtr.h"
#include "Callback.h"

#include <stdint.h>

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers.
 * Die Server-interne Repräsentation des FID für einen Zugriff.
 */
class Fid
{
public:
  Fid();
  virtual
  ~Fid();

  uint32_t fid;
  unsigned refcount;
  uint16_t omode; // replicates the open mode of a fid in use
  const NpUser*   user;

  RefCntPtr<Directory> directory;
  RefCntPtr<Resource> dirEntry;
  IoState* ioRef;
  const Callback<bool, Directory*>* nsDetach;

  void incRef();
  void decRef();

  void clone(const Fid* other);
  void moveTo(Fid** otherPool);

  bool isAuth() const;

  void* operator new(size_t size, MemoryManager* mm) throw();
  void operator delete(void * mem) throw();

  static Fid* create(Fid** pool, uint32_t fid, MemoryManager* mm);
  static Fid* find(Fid** pool, uint32_t fid);
  static Fid* getFirst(Fid** pool);
  static Fid** createPool(MemoryManager* mm);
  static void destroyPool(Fid** pool, MemoryManager* mm);

  static const uint32_t NoFid = (uint32_t)(~0);
  static const unsigned LookupTableSize = 16;
private:
  Fid *next;
  Fid **pool;
};

inline
bool Fid::isAuth() const
{
  return false;
}

inline
void * Fid::operator new(size_t size, MemoryManager* mm) throw()
{
  return npfsNew(size, mm);
}

inline
void Fid::operator delete(void *mem) throw()
{
  npfsDelete(mem);
}

}

#endif /* FID_H_ */
