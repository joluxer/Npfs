/*
 * ByteStreamFile.h
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#ifndef BYTESTREAMFILE_H_
#define BYTESTREAMFILE_H_

#include "ByteStream.h"

#include "../Npfs/Resources/Resource.h"

namespace Npfs
{

/***
 * Diese Klasse stellt einen ByteStream als NPFS-Datei bereit.
 *
 * Dabei wird zwischen Input-Stream (read-Operationen) und Output-Stream (write-Operationen)
 * unterschieden. Diese beiden ByteStreams können identisch sein, müssen es aber nicht.
 *
 * Bei der Konstruktion des Objektes wird für die Festlegung der Zugriffsrechte
 * berücksichtigt, ob je ein Input- bzw. Output-Datenstrom verfügbar ist. Die
 * modifyStat()-Implementation berücksichtigt dies noch nicht.
 *
 * Die Übertragung der Daten ist nicht blockierend. Das bedeutet, daß nur so viele Daten
 * ausgegeben werden, wie der Ausgabe-Strom aufnehmen kann, bzw. nur so viele Daten eingelesen
 * werden, wie der Eingabe-Datenstrom verfügbar hat. Eine Ausnahme davon ist, wenn der ByteStream
 * keine Daten liefert, dann bekommt der Client erst seine Rmessage, wenn wieder Daten verfügbar sind.
 * Diese Situation kann umgestellt werden, so daß stattdessen Rerror("Ressource temporarily unavailable") gemeldet wird.
 */
class ByteStreamFile: public Npfs::Resource
{
public:
  /***
   * Der Konstruktor erzeugt da Objekt und legt seine (initialen) Eigenschaften fest.
   *
   * @param filename ist der Dateiname im File-System, unter dem die ByteStreams ansprechbar sind.
   * @param parent   ist das übergeordnete Verzeichnis, in dem die Datei bereitgestellt wird; kann Null sein, dann muß die Datei später gemounted werden.
   * @param output   gibt den Output-ByteStream für write-Operationen an, kann Null sein, dann wird kein Output zugelassen, kann mit dem Input-ByteStream identisch sein.
   * @param input    gibt den Input-ByteStream für read-Operationen an, kann Null sein, dann wird kein Input zugelassen, kann mit dem Output-ByteStream identisch sein.
   * @param umask    gibt an, welche Zugriffsrechte initial ausgeblendet werden sollen, wird mit der Verfügbarkeit von Input und Output verknüpft.
   * @param blockOnZeroInput gibt an, ob eine Rerror-Nachricht (temporarily not available)) geschickt wird (false) oder die Rmessage verzögert wird, bis wieder Daten verfügbar sind (true).
   */
  ByteStreamFile(const char* filename, Npfs::Directory* parent, ByteStream* output, ByteStream* input, uint16_t umask = 0133, bool blockOnZeroInput = true);
//  virtual ~ByteStreamFile();

  void setZeroInputBlocking(bool block = true);

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
  ResultMessage trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength);   ///< truncations to zero are allowed

protected:
  ByteStream *outputStream, *inputStream;
  bool blockOnZeroInput:1;
};

inline
void ByteStreamFile::setZeroInputBlocking(bool block)
{
  blockOnZeroInput = block;
}

}

#endif /* BYTESTREAMFILE_H_ */
