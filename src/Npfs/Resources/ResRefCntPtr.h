/*
 * ResRefCntPtr.h
 *
 *  Created on: 10.04.2015
 *      Author: lode
 */

#ifndef RESREFCNTPTR_H_
#define RESREFCNTPTR_H_

template<typename T> class RefCntPtr
{
private:
  T* pData; // pointer

public:
  RefCntPtr()
  : pData(0)
  {}

  RefCntPtr(T* pValue)
  : pData(pValue)
  {
    if (pData)
      pData->incrAccessRef();
  }

  // copy constructor
  RefCntPtr(const RefCntPtr<T>& other)
  : pData(other.pData)
  {
    // Copy the data pointer
    // and increment the reference count
    if (pData)
      pData->incrAccessRef();
  }

#if __cplusplus >= 201103L
  // move constructor
  RefCntPtr(RefCntPtr<T>&& other)
  : pData(other.pData)
  {
    // Copy the data pointer
    // and leave the reference count
    // pData->incrAccessRef();
    other.pData = 0;
  }
#endif

  ~RefCntPtr()
  {
    // Decrement the reference count
    if (pData)
      pData->decrAccessRef();
  }

  T& operator*()
  {
    return *pData;
  }

  T* operator->()
  {
    return pData;
  }

  RefCntPtr<T>& operator =(const RefCntPtr<T>& other)
  {
    // Assignment operator
    if (this != &other) // Avoid self assignment
    {
      // Decrement the old reference count
      if (pData)
        pData->decrAccessRef();

      // Copy the data and reference pointer
      // and increment the reference count
      pData = other.pData;
      if (pData)
        pData->incrAccessRef();
    }
    return *this;
  }

#if __cplusplus >= 201103L
  RefCntPtr<T>& operator =(RefCntPtr<T>&& other)
  {
    // Assignment move operator
    if (this != &other) // Avoid self assignment
    {
      // Decrement the old reference count
      if (pData)
        pData->decrAccessRef();

      // Copy the data pointer
      pData = other.pData;
      other.pData = 0;
    }
    return *this;
  }
#endif

#if __cplusplus >= 201103L
  explicit operator bool() const noexcept
#else
  operator bool() const
#endif
  {
    return !!pData;
  }

#if __cplusplus >= 201103L
  T* get() const noexcept
#else
  T* get() const
#endif
  {
    return pData;
  }

#if __cplusplus >= 201103L
  T* release() noexcept
#else
  T* release()
#endif
  {
    auto ptr = pData;
    if (pData)
      pData->decrAccessRef();
    pData = 0;
    return ptr;
  }

};

#endif /* RESREFCNTPTR_H_ */
