/*
 * IoPinFile.h
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#ifndef IOPINFILE_H_
#define IOPINFILE_H_

#include "IoPin.h"

#include "Resource.h"

#include "Callback.h"

namespace Npfs
{

/***
 * Diese Klasse stellt ein String-Interface für eine zweiwertige Variable vom Typ IoPin bereit.
 *
 * Der Wert der Variable wird beim Lesen der Datei als String zurückgegeben (State), evtl. zusammen mit der
 * Angabe, welche Strings zur Änderung des Zustands zulässig sind (Commands).
 *
 * Die Ausgabe ist konfigurierbar, so daß der Text "status: " und "commands: " durch andere Texte ersetzt
 * oder entfernt werden kann. Auch die Strings, welche den Zustand der Variable beschreiben
 * ("active" und "passive") sind konfigurierbar.
 *
 * Beim Schließen der Datei kann ein Callback gerufen werden, wenn der Wert der Variable geändert wurde.
 *
 * Die in die Datei geschriebenen Strings können abgekürzt werden, solange sie eindeutig sind. Wird ein
 * komplett nicht zutreffender String geschrieben, gibt die write()-Methode "Permission denied" zurück.
 *
 * Für dynamische Laufzeit-Daten benötigt die Klasse einen statisch zugewiesenen MemoryManager.
 */
class IoPinFile: public Npfs::Resource
{
public:
  IoPinFile(const char* filename, Npfs::Directory* parent, IoPin& variable, uint16_t umask = 0177);
//  virtual ~IoPinFile();

  static void useMemory(MemoryManager* mm); ///< @note All files of this type use the same MemoryManager

  static const char DefaultActiveString[];
  static const char DefaultPassivString[];
  static const char DefaultStateString[];
  static const char DefaultCommandsString[];

  struct Strings
  {
    const char* activeString;
    const char* passiveString;
    const char* statePrefix;
    const char* commandsPrefix;

    constexpr Strings(const char* active = DefaultActiveString, const char* passive = DefaultPassivString, const char* state = DefaultStateString, const char* commands = DefaultCommandsString);
  };

  static const Strings DefaultStrings;
  static const Strings OnOffStrings;

  /***
   * Mit dieser Methode sind die Strings, die den Datei-Inhalt darstellen konfigurierbar.
   *
   * Die beiden Zustandsbeschreibungen der Variable sind als True-String und als False-String einstellbar,
   * sowie die beiden Präfix-Text vor der Zustands-Angabe und vor der Ausgabe der möglichen Kommandos.
   *
   * Wird der activeString bzw. der passiveString mit 0 angegeben, wird standardmäßig "true" bzw. "false" eingesetzt.
   *
   * Wird der statePrefix mit 0 angegeben, wird ein leerer Präfix-String ausgegeben.
   *
   * Wird der commandPräfix mit 0 angegeben, wird die List der gülitgen Kommandos nicht ausgegeben.
   *
   * @param activeString
   * @param passiveString
   * @param statePrefix
   * @param commandPrefix
   */
  void setStrings(const Strings& ioStrings);
  void setChangeHandler(const Callback<void, bool>* handler); ///< the given handler will be executed on closing the file once

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

protected:
  struct IoBuffer: public Npfs::IoState
  {
    const char* readValue;
    unsigned readOffset;
    unsigned writeOffset;
    struct _Flags
    {
      bool writeActiveOk:1;
      bool writePassiveOk:1;
      bool isWritten:1;
    } flag;
  };

  IoPin& myIoPin;
  const Strings* string;

  const Callback<void, bool>* changeHandler;

  virtual
  IoBuffer* makeIoBuffer();

  virtual
  bool blockRead(IoBuffer& workRef); // wird in open() gerufen, gibt true zurück, wenn der read()-Aufruf blockiert werden soll, z.B. bis der Input ein Flanke erfüllt, die Default-Implementation gibt false zurück

  virtual
  bool releaseRead(IoBuffer& workRef); // wird in read() gerufen, wenn read() blockiert, gibt true zurück, wenn der read()-Aufruf den Wert lesen soll, die Default-Implementation gibt true zurück

  virtual
  const char* readValue(IoBuffer& workRef); // wird in open() oder read() gerufen, abhängig davon, ob blockRead() false oder true ergibt, gibt den Wert zurück, der an den Nutzer ausgegeben werden soll, die Default-Implementation gibt den aktuellen Wert für das IoPin zurück

  virtual
  bool isWriteAccepted(IoBuffer& workRef, const unsigned char* buffer, unsigned bufLen, uint64_t offset);  // hier kann eine abgeleitete Klasse weitere Strings und Aktionen akzeptieren; die Default-Implementation gibt false zurück

  static MemoryManager* mm;
};

inline
void IoPinFile::useMemory(MemoryManager* mem)
{
  mm = mem;
}

inline
void IoPinFile::setStrings(const Strings& ioStrings)
{
  string = &ioStrings;
}

inline
void IoPinFile::setChangeHandler(const Callback<void, bool>* handler)
{
  changeHandler = handler;
}

inline
constexpr IoPinFile::Strings::Strings(const char* active, const char* passive, const char* state, const char* commands)
: activeString(active ? active : DefaultActiveString), passiveString(passive ? passive : DefaultPassivString), statePrefix(state), commandsPrefix(commands)
{}

}

#endif /* IOPINFILE_H_ */
