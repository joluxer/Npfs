/*
 * MemoryManager.h
 *
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include <stdlib.h>


/**
 * This class represents a simple memory manager interface.
 *
 * Implementors of this interface simply provide chunks of memory from an pool.
 */
class MemoryManager
{
public:
//  MemoryManager();
  virtual ~MemoryManager();

  virtual void* allocate(size_t) = 0;
  virtual void  release(const void*) = 0;

  // make it usable as Alloc class for ::std::allocator_traits
  template<typename T>
  void deallocate(T* ptr) { release(reinterpret_cast<const void*>(ptr)); }

  template<typename T>
  struct Deleter
  {
    void operator()(T* p)
    {
      p->~T();
      mm->release(p);
    }
  private:
    MemoryManager* mm;
    friend class MemoryManger;
  };

  template<typename T>
  Deleter<T> getDeleter()
  {
    Deleter<T> d;
    d.mm = this;

    return d;
  }
};

template<typename T, MemoryManager* MM>
struct MemoryManagerAllocator
{
  typedef T value_type;
  MemoryManagerAllocator() noexcept {}
  template <class U> MemoryManagerAllocator (const MemoryManagerAllocator<U, MM>&) noexcept {}
  T* allocate (std::size_t n) { return static_cast<T*>(MM->allocate(n*sizeof(T))); }
  void deallocate (T* p, std::size_t n) { MM->deallocate(p); }
};

#endif /* MEMORYMANAGER_H_ */
