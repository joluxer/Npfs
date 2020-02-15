/*
 * Fid.cpp
 *
 *  Created on: 25.07.2012
 *      Author: jlode
 */

#include "Fid.h"

namespace Npfs
{

Fid::Fid()
: fid(~0), refcount(0), omode(~0),
  user(0), directory(0), dirEntry(0), ioRef(0),
  nsDetach(0)
{}

Fid::~Fid()
{
  int hash;
  Fid **htable, *f, **prevp;

  if (ioRef)  // normally the resource implementation shall cleanup these pointers, so this is a last resort, which might go wrong, as this is not nessessarily deletable
    delete ioRef;

  hash = fid % LookupTableSize;
  htable = pool;
  if (htable)
  {
    prevp = &htable[hash];
    f = *prevp;
    while (f != 0)
    {
      if (f->fid == this->fid)
      {
        *prevp = f->next;
        break;
      }

      prevp = &f->next;
      f = *prevp;
    }
  }
}

Fid** Fid::createPool(MemoryManager* mm)
{
  unsigned i;
  Fid **nPool = (Fid**)mm->allocate(sizeof(Fid*) * LookupTableSize);
  if (nPool)
  {
    for (i = 0; i < LookupTableSize; i++)
    {
      nPool[i] = 0;
    }
  }
  return nPool;
}

void Fid::destroyPool(Fid** pool, MemoryManager* mm)
{
  if (pool)
  {
    mm->release(pool);
  }
}

Fid* Fid::find(Fid** pool, uint32_t fid)
{
  int hash;
  Fid **htable, *f, **prevp;

  hash = fid % LookupTableSize;
  htable = pool;
  if (!htable)
    return 0;

  prevp = &htable[hash];
  f = *prevp;
  while (f != 0)
  {
    if (f->fid == fid)
    {
      *prevp = f->next;
      f->next = htable[hash];
      htable[hash] = f;
      break;
    }

    prevp = &f->next;
    f = *prevp;
  }
  return f;
}

Fid* Fid::create(Fid**pool, uint32_t fid, MemoryManager* mm)
{
  int hash;
  Fid **htable, *f;

  hash = fid % LookupTableSize;
  htable = pool;
  if (!htable)
    return 0;

  f = find(pool, fid);
  if (f)
    return 0;

  f = new(mm) Fid;
  if (!f)
    return 0;

  f->fid = fid;
  f->refcount = 0;
  f->user = 0;

  f->pool = pool;
  f->next = htable[hash];
  htable[hash] = f;

  return f;
}

void Fid::incRef()
{
  refcount++;
}

void Fid::decRef()
{
  if (refcount)
    refcount--;
  else
    while(1);

  if (0 == refcount)
  {
    delete this;
  }
}

void Fid::clone(const Fid* other)
{
  omode = ~0;
  user = other->user;

  directory = other->directory;
  dirEntry = other->dirEntry;
}

Fid* Fid::getFirst(Fid** pool)
{
  unsigned i;
  Fid* f = 0;

  for (i = 0; i < LookupTableSize; i++)
  {
    if (pool[i])
    {
      f = pool[i];
      break;
    }
  }

  return f;
}

void Fid::moveTo(Fid ** newPool)
{
  int hash;
  Fid **htable, *f, **prevp;

  hash = fid % LookupTableSize;

  // aus altem Pool entfernen
  htable = pool;
  if (htable)
  {
    prevp = &htable[hash];
    f = *prevp;
    while (f != 0)
    {
      if (f->fid == this->fid)
      {
        // gefunden, entfernen
        *prevp = f->next;
        break;
      }

      prevp = &f->next;
      f = *prevp;
    }
  }

  // in neuen Pool einfügen
  pool = newPool;
  htable = pool;

  next = htable[hash];
  htable[hash] = this;
}

}
