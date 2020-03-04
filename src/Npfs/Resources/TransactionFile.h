/*
 * TransactionFile.h
 *
 *  Created on: 16.08.2012
 *      Author: jlode
 */

#ifndef TRANSACTIONFILE_H_
#define TRANSACTIONFILE_H_

#include "Resource.h"
#include "Callback.h"
#include "MemoryManager.h"
#include "pt/pt.h"

#include <string.h>
#include <assert.h>

namespace Npfs
{

//struct SerDes
//{
//  static const bool canSerialize = false;
//  static const bool canDeserialize = false;
//  static const uint64_t DataLength_bytes = 0;
//  typedef NpStrBufferPtr VarType;
//  typedef NpStrBuffer<StringLength> VarConstructionType;
//
//  inline
//  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength) {};
//
//  inline
//  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength) {};
//};

/**
 * This class template presents the serialized value of a scalar variable as 9P2000 file and can
 * deserialize the given data stream into the variable. The varaiable lives only as long as the file
 * is open. It is created on open() and destroyed on close()/flush(). Thus the file content is
 * volatile, but will be created, i.e. for an transaction or something simelar and will
 * be destroyed after processing of the new content after the close() call.
 *
 * The size of the sent and received amount of data depends on the SerDes class only and is static.
 * Thus, the file size is the same for all values of the variable and for read and write.
 *
 * This class template is intended for small sized representations of the variable value, as in
 * open() will be an uchar buffer allocated which represents the serialized value. This buffer lives
 * until close().
 *
 * The access to the variable value starts with open(), where the serialized representation is fixed. All
 * read() and write() accesses modify the serialized buffer like a normal file. On close() the buffer
 * is deserialized, if it was changed (i.e. write() was called). After the close() call, the variable
 * receives the new value and the modification handler is notified.
 *
 * @param VarType is the type of the variable to be served
 * @param SerDes is a serializer deserializer class for the VarType, that accepts (probably) an uchar stream and builds a new value from it
 * and produces an uchar stream from the variable value during serialization.
 */
template<class SerDes>
class TransactionFile: public Npfs::Resource
{
public:
  /**
   *
   * @param filename    The name of the file.
   * @param parent      The containing directory.
   * @param umask       The umask for the initial access values (every bit from the umask is initially kept unset in the stat).
   */
  TransactionFile(const char* filename, Npfs::Directory* parent, uint16_t umask = 0177);

  static void useMemory(MemoryManager* mm); ///< @note All files of this type use the same MemoryManager

  virtual
  ResultMessage open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit);
  virtual
  ResultMessage read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset);  // in count will be returned the actual read amount of data, the default implementation refuses the action
  virtual
  ResultMessage write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset); // the default implementation refuses the action
  virtual
  void flush(Npfs::OpenIoState& workRef);  ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method, this call blocks, until flush completes, may be called multiple times for the same operation
  virtual
  ResultMessage close(Npfs::OpenIoState& workRef);  // the default implementation just deletes ioState and returns, will be called only, if workRef.ioState is not zero
//  virtual
//  ResultMessage remove(Npfs::BlockingOpState* &opRef, const Npfs::NpUser& user); // the resource is asked to remove itself, which in turn must ask its parent to remove the entry...; the default implementation refuses the action
//  virtual
//  const NpStat& getStat();
//  virtual
//  ResultMessage modifyStat(Npfs::BlockingOpState* &opRef, const Npfs::NpStat& wstat); // the default implementation modifies stat according the rules from 9P
  virtual
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);

  void setBeginHandler(const Callback<bool, typename SerDes::VarType&, uint8_t, const char*&, void*>* handler, void* ref = reinterpret_cast<void*>(~uintptr_t(0))); ///< the given handler will be executed on opening the file, until it returns true, second arg is the open mode (Oread, Owrite, Ordwr), third arg is returned error msg, if not nullptr
  void setAbortHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref = reinterpret_cast<void*>(~uintptr_t(0))); ///< the given handler will be executed once on closing the file with flush(), return value will be ignored, second arg is true, if file was opened for write, third arg is ignored, return value is ignored
  void setCommitHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref = reinterpret_cast<void*>(~uintptr_t(0))); ///< the given handler will be executed on closing the file, if the file was opened for write, just before the endHandler, until it returns true, second arg is true, if file was actually written to, third arg is returned error msg, if not nullptr
  void setEndHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref = reinterpret_cast<void*>(~uintptr_t(0))); ///< the given handler will be executed on closing the file, until it returns true, second arg is true if file was opened for write, third arg is returned error msg, if not nullptr

protected:
  const Callback<bool, typename SerDes::VarType&, uint8_t, const char*&, void*>* beginTxnHandler;
  void* beginTxnRef;
  const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>*  abortTxnHandler;
  void* abortTxnRef;
  const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>*  commitTxnHandler;
  void* commitTxnRef;
  const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>*  endTxnHandler;
  void* endTxnRef;

  struct IoBuffer: public Npfs::IoState
  {
    IoBuffer();
    ~IoBuffer();
    typename SerDes::VarConstructionType transientValue;
    unsigned char *ioBuffer;
    unsigned bufferFill;

    bool wasOpenedForWrite:1;
    bool isWritten:1;
  };

  struct CoRoState: public Npfs::BlockingOpState
  {
    struct pt ptState;
  };

  static MemoryManager* mm;

  PT_THREAD(openCoRoutine(ResultMessage& result, Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit));
  void serialize(IoBuffer* ioRef);
  PT_THREAD(closeCoRoutine(ResultMessage& result, Npfs::OpenIoState& workRef));
  bool deserialize(IoBuffer* ioRef);
};

template<class SerDes>
void TransactionFile<SerDes>::useMemory(MemoryManager* mm_)
{
  mm = mm_;
}

template<class SerDes>
void TransactionFile<SerDes>::setBeginHandler(const Callback<bool, typename SerDes::VarType&, uint8_t, const char*&, void*>* handler, void* ref)
{
  beginTxnHandler = handler;
  beginTxnRef = ref;
}

template<class SerDes>
void TransactionFile<SerDes>::setAbortHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref)
{
  abortTxnHandler = handler;
  abortTxnRef = ref;
}

template<class SerDes>
void TransactionFile<SerDes>::setCommitHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref)
{
  commitTxnHandler = handler;
  commitTxnRef = ref;
}

template<class SerDes>
void TransactionFile<SerDes>::setEndHandler(const Callback<bool, typename SerDes::VarType&, bool, const char*&, void*>* handler, void* ref)
{
  endTxnHandler = handler;
  endTxnRef = ref;
}

}

#endif /* TRANSACTIONFILE_H_ */
