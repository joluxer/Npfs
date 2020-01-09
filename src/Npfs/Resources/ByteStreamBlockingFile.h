/*
 * ByteStreamBlockingFile.h
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#ifndef BYTESTREAMBLOCKINGFILE_H_
#define BYTESTREAMBLOCKINGFILE_H_

#include "ByteStream.h"

#include "../Npfs/Resources/Resource.h"
#include "MemoryManager.h"

namespace Npfs
{

/***
 * Diese Klasse stellt einen ByteStream als NPFS-Datei bereit.
 *
 * Dabei wird zwischen Input-Stream und Output-Stream unterschieden. Diese
 * beiden ByteStreams können identisch sein, müssen es aber nicht.
 *
 * Bei der Konstruktion des Objektes wird für die Festlegung der Zugriffsrechte
 * berücksichtigt, ob je ein Input- bzw. Output-Datenstrom verfügbar ist. Die
 * modifyStat()-Implementation berücksichtigt dies noch nicht.
 *
 * Die Übertragung der Daten ist blockierend. Das bedeutet, daß der Client seine
 * Antwort (Rmessage) erst erhält, wenn alle Daten ausgegeben sind bzw. der Lesepuffer
 * gefüllt ist.
 */
class ByteStreamBlockingFile: public Npfs::Resource
{
public:
  /***
   * Der Konstruktor erzeugt da Objekt und legt seine (initialen) Eigenschaften fest.
   *
   * @param filename ist der Dateiname im File-System, unter dem die ByteStreams ansprechbar sind.
   * @param parent   ist das übegeordnete Verzeichnis, in dem die Datei bereitgestellt wird; kann Null sein, dann muß die Datei später gemounted werden.
   * @param output   gibt den Output-ByteStream an, kann Null sein, dann wird kein Output zugelassen, kann mit dem Input-ByteStream identisch sein.
   * @param input    gibt den Input-ByteStream an, kann Null sein, dann wird kein Input zugelassen, kann mit dem Input-ByteStream identisch sein.
   * @param umask    gibt an, welche Zugriffsrechte initial ausgeblendet werden sollen, wird mit der Verfügbarkeit von Input und Output verknüpft.
   */
  ByteStreamBlockingFile(const char* filename, Npfs::Directory* parent, ByteStream* fileWriteStream, ByteStream* fileReadStream, uint16_t umask = 0133);
//  virtual ~ByteStreamBlockingFile();

  static void useMemory(MemoryManager* mm); ///< all files of this type use the same MemoryManager

  virtual
  ResultMessage open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit);
  virtual
  ResultMessage read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset);  // in count will be returned the actual read amount of data, the default implementation refuses the action
  virtual
  ResultMessage write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset); // the default implementation refuses the action
  virtual
  void flush(Npfs::OpenIoState& workRef);  ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method, this call blocks, until flush completes, may be called multiple times for the same operation
//  virtual
//  ResultMessage close(Npfs::OpenIoState& workRef);  // the default implementation just deletes ioState and returns, will be called only, if workRef.ioState is not zero
//  virtual
//  ResultMessage remove(Npfs::BlockingOpState* &opRef, const Npfs::NpUser& user); // the resource is asked to remove itself, which in turn must ask its parent to remove the entry...; the default implementation refuses the action
//  virtual
//  const NpStat& getStat();
//  virtual
//  ResultMessage modifyStat(Npfs::BlockingOpState* &opRef, const Npfs::NpStat& wstat); // the default implementation modifies stat according the rules from 9P
  virtual
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);   // the default implementation refuses truncations

protected:
  ByteStream *writeStream, *readStream;

  struct OpState: public Npfs::BlockingOpState
  {
    uint32_t bytesDone;
  };

  static MemoryManager* mm;

};

inline
void ByteStreamBlockingFile::useMemory(MemoryManager* _mm)
{
  mm = _mm;
}

}

#endif /* BYTESTREAMBLOCKINGFILE_H_ */
