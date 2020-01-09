/*
 * StdStringFile.h
 *
 *  Created on: 06.08.2012
 *      Author: jlode
 */

#ifndef STDSTRINGFILE_H_
#define STDSTRINGFILE_H_

#include "Npfs/Resources/Resource.h"
#include "Callback.h"
#include "MemoryManager.h"

#include <string>

namespace Npfs
{

/**
 * Diese Klasse stellt einen std::string als NPFS-Datei bereit.
 *
 */
class StdStringFile: public Npfs::Resource
{
public:
  /**
   * Der Konstruktor legt fest, wie das gesamte Objekt beschaffen ist.
   *
   * @param filename ist der initiale Datei-Name für die Datei
   * @param parent   ist das übergeordnete Verzeichnis, in dem die Datei angelegt wird.
   * @param storage ist der bereitgestellte std::string, in dem die Daten der Datei abgelegt werden.
   * @param umask gibt an, welche Zugriffrechte für die Datei initial entfernt werden sollen.
   */
  StdStringFile(const char* filename, Npfs::Directory* parent, std::string& storage, uint16_t umask = 0133);
//  virtual ~StdStringFile();

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
  virtual
  const NpStat& getStat();
//  virtual
//  ResultMessage modifyStat(Npfs::BlockingOpState* &opRef, const Npfs::NpStat& wstat); // the default implementation modifies stat according the rules from 9P
  virtual
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);   // the default implementation refuses truncations

  void setUpdateHandler(const Callback<bool, std::string&, uint64_t&>* handler); ///< the given handler will be executed on opening the file, until it returns true. Provided arguments are the file data buffer and a writable reference to the file size.
  void setChangeHandler(const Callback<bool, std::string&, uint64_t&>* handler); ///< the given handler will be executed on closing the file, until it returns true. Provided arguments are the file data buffer and a writable reference to the file size.
  void setTruncHandler(const Callback<bool, std::string&, uint64_t&>* handler);  ///< the given handler will be executed on size changes, until it returns true. Provided arguments are the file data buffer and a writable reference to the new file size.
  void setModifiedHandler(const Callback<bool, std::string&, uint32_t&, uint64_t&>* handler);///< the given handler will be executed once after a write access, return value is ignored. Provided arguments are the start of the modified file data and modification length. @note Changes of the modification length are ignored by this class.

protected:

  struct IoRefState: public Npfs::IoState
  {
    bool isWritten;

    void * operator new(size_t size) throw();
    void operator delete(void *mem) throw();
  };

  std::string& fileData;
  const Callback<bool, std::string&, uint64_t&>* updateHandler;
  const Callback<bool, std::string&, uint64_t&>* changeHandler;
  const Callback<bool, std::string&, uint64_t&>* truncHandler;
  const Callback<bool, std::string&, uint32_t&, uint64_t&>* modifiedHandler;
};

inline
void StdStringFile::setUpdateHandler(const Callback<bool, std::string&, uint64_t&>* handler)
{
  updateHandler = handler;
}

inline
void StdStringFile::setChangeHandler(const Callback<bool, std::string&, uint64_t&>* handler)
{
  changeHandler = handler;
}

inline
void StdStringFile::setTruncHandler(const Callback<bool, std::string&, uint64_t&>* handler)
{
  truncHandler = handler;
}

inline
void StdStringFile::setModifiedHandler(const Callback<bool, std::string&, uint32_t&, uint64_t&>* handler)
{
  modifiedHandler = handler;
}

}

#endif /* STDSTRINGFILE_H_ */
