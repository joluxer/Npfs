/*
 * RamFile.h
 *
 *  Created on: 06.08.2012
 *      Author: jlode
 */

#ifndef RAMFILE_H_
#define RAMFILE_H_

#include "Resource.h"
#include "Callback.h"
#include "MemoryManager.h"

namespace Npfs
{

/**
 * Diese Klasse stellt einen RAM-Bereich als NPFS-Datei bereit.
 *
 * Die Datei kann maximal die Größe des, bei der Konstruktion des Objektes bereitgestellten,
 * Speicherbereiches haben; sie kann auch auf jeden beliebigen Wert bis Null hinunter gekürzt werden.
 * In diesem Fall wird schlicht nicht der gesamte bestellte Speicherbereich genutzt, sondern nur der
 * notwendige, beginnend bei Offset 0.
 */
class RamFile: public Npfs::Resource
{
public:
  /**
   * Der Konstruktor legt fest, wie das gesamte Objekt beschaffen ist.
   *
   * Dazu gehört die maximale Größe der Datei, wo die Daten abgelegt werden und wie der Datei-Name ist.
   *
   * Der übergebene Speicherbereich wird zusammenhängend in der angegebenen maximalen Größe verwendet.
   *
   * @note Der angegebene Datentyp bestimmt, wie auf den Speicher zugegriffen wird. uint8_t Typen erzeugen Zugriffe
   * mittels der memcpy-Funktion, alle anderen Typen sorgen für einen Zugriff in exakt der angegebenen Zellengröße.
   * Non-Volatile-Type werden mit Nullen aufgefüllt, wenn die Zugriffe mit Offsets größer als stat.length zu Lücken
   * führen würden.
   * Volatile-Typen werden sparse behandelt, d.h. daß Offsets, die größer sind als die stat.length führen zu Lücken, die nicht aufgefüllt.
   *
   * @param filename ist der initiale Datei-Name für die Datei
   * @param parent   ist das übergeordnete Verzeichnis, in dem die Datei angelegt wird.
   * @param maxFileSize ist die größe des bereitgestellten Speicherbereiches und damit gleichzeitig die maximale Größe der Datei im File-System
   * @param fileArea ist der bereitgestellte Speicherbereich im RAM, in dem die Daten der Datei abgelegt werden.
   * @param umask gibt an, welche Zugriffrechte für die Datei initial entfernt werden sollen.
   */
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, unsigned char* fileArea, uint16_t umask = 0133);
//  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, uint8_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, uint16_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, uint32_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, uint64_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, volatile uint8_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, volatile uint16_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, volatile uint32_t* fileArea, uint16_t umask = 0133);
  RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize_words, volatile uint64_t* fileArea, uint16_t umask = 0133);
//  virtual ~RamFile();

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
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);   // the default implementation refuses truncations

  void setUpdateHandler(const Callback<bool, unsigned char*, uint64_t&>* handler); ///< the given handler will be executed on opening the file, until it returns true. Provided arguments are the file data buffer and a writable reference to the file size.
  void setChangeHandler(const Callback<bool, unsigned char*, uint64_t&>* handler); ///< the given handler will be executed on closing the file, until it returns true. Provided arguments are the file data buffer and a writable reference to the file size.
  void setTruncHandler(const Callback<bool, unsigned char*, uint64_t&>* handler);  ///< the given handler will be executed on size changes, until it returns true. Provided arguments are the file data buffer and a writable reference to the new file size.
  void setModifiedHandler(const Callback<bool, unsigned char*, uint32_t&>* handler);///< the given handler will be executed once after a write access, return value is ignored. Provided arguments are the start of the modified file data and modification length. @note Changes of the modification length are ignored by this class.

  enum AccessType
  {
    UnalignedZeroFilled, UnalignedSparse,
    Uint8AlignedZeroFilled, Uint8AlignedSparse,
    Uint16AlignedZeroFilled, Uint16AlignedSparse,
    Uint32AlignedZeroFilled, Uint32AlignedSparse,
    Uint64AlignedZeroFilled, Uint64AlignedSparse,
  };

  /**
   * This method sets the exact access size for the addressed cells.
   *
   * Unaligned means, the systems memcpy() function is used, any memAccess uses exact the given access size.
   * Zero filled files, which are smaller than the maximum size, fills the gaps with zeros, if accessed with offsets, that produce gaps.
   * Sparse accesses leave the gaps untouched.
   * Sparse access is default for volatile memory areas, zero filled for non-volatile.
   *
   * @param acc
   */
  void setAccess(AccessType acc);

  template<typename WordType>
  void changeBaseAddress(WordType *newFileAreaBase, unsigned newMaxFileSize = 0);

protected:

  struct IoRefState: public Npfs::IoState
  {
    bool isWritten;
  };

  unsigned char *fileData;
  unsigned maxDataSize;
  AccessType memAccess;
  const Callback<bool, unsigned char*, uint64_t&>* updateHandler;
  const Callback<bool, unsigned char*, uint64_t&>* changeHandler;
  const Callback<bool, unsigned char*, uint64_t&>* truncHandler;
  const Callback<bool, unsigned char*, uint32_t&>* modifiedHandler;

  void copyUnaligned(unsigned char *dataBuffer, const unsigned char *fileData, uint32_t count);
  template<typename T>
  void copyFromFileAlignedFor(unsigned char *dataBuffer, const unsigned char *fileData, uint32_t count);
  template<typename T>
  void copyToFileAlignedFor(volatile unsigned char *fileData, const unsigned char *dataBuffer, uint32_t count);

  static MemoryManager* mm;
};

inline
void RamFile::setUpdateHandler(const Callback<bool, unsigned char*, uint64_t&>* handler)
{
  updateHandler = handler;
}

inline
void RamFile::setChangeHandler(const Callback<bool, unsigned char*, uint64_t&>* handler)
{
  changeHandler = handler;
}

inline
void RamFile::setTruncHandler(const Callback<bool, unsigned char*, uint64_t&>* handler)
{
  truncHandler = handler;
}

inline
void RamFile::setModifiedHandler(const Callback<bool, unsigned char*, uint32_t&>* handler)
{
  modifiedHandler = handler;
}

inline
void RamFile::useMemory(MemoryManager* mm_)
{
  mm = mm_;
}

inline
void RamFile::setAccess(AccessType acc)
{
  memAccess = acc;
}

template<typename WordType>
inline
void RamFile::changeBaseAddress(WordType *newFileAreaBase, unsigned newMaxFileSize)
{
  fileData = reinterpret_cast<decltype(fileData)>(newFileAreaBase);
  if (newMaxFileSize)
    maxDataSize = newMaxFileSize;
}

}

#endif /* RAMFILE_H_ */
