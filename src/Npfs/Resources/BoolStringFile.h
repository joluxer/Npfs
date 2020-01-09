/*
 * BoolStringFile.h
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#ifndef BOOLSTRINGFILEFILE_H_
#define BOOLSTRINGFILEFILE_H_

#include "Resource.h"

#include "Callback.h"

namespace Npfs
{

/***
 * Diese Klasse stellt ein String-Interface für eine zweiwertige Variable vom Typ bool bereit.
 *
 * Der Wert der Variable wird beim Lesen der Datei als String zurückgegeben (State), evtl. zusammen mit der
 * Angabe, welche Strings zur Änderung des Zustands zulässig sind (Commands).
 *
 * Die Ausgabe ist konfigurierbar, so daß der Text "status: " und "commands: " durch andere Texte ersetzt
 * oder entfernt werden kann. Auch die Strings, welche den Zustand der Variable beschreiben
 * ("true" und "false") sind konfigurierbar.
 *
 * Beim Öffnen der Datei kann die Variable über einen Callback aktualisiert werden. Beim Schließen der Datei
 * kann ein Callback gerufen werden, wenn der Wert der Variable geändert wurde.
 *
 * Die in die Datei geschriebenen String können abgekürzt werden, solange sie eindeutig sind. Wird ein
 * komplett nicht zutreffender String geschrieben, gibt die write()-Methode "Permission denied" zurück.
 *
 * Für dynamische Laufzeit-Daten benötigt die Klasse einen statisch zugewiesenen MemoryManager.
 */
class BoolStringFile: public Npfs::Resource
{
public:
  BoolStringFile(const char* filename, Npfs::Directory* parent, bool& variable, uint16_t umask = 0133);
//  virtual ~BoolStringFile();

  static void useMemory(MemoryManager* mm); ///< @note All files of this type use the same MemoryManager

  static const char DefaultTrueString[];
  static const char DefaultFalseString[];
  static const char DefaultSetTrueString[];
  static const char DefaultSetFalseString[];
  static const char DefaultStateString[];
  static const char DefaultCommandsString[];

  struct Strings
  {
    const char* trueString;
    const char* falseString;
    const char* setTrueCommand;
    const char* setFalseCommand;
    const char* statusPrefix;
    const char* commandsPrefix;

    constexpr
    Strings(const char* active = DefaultTrueString, const char* passive = DefaultFalseString, const char* setTrue = DefaultSetTrueString, const char* setFalse = DefaultSetFalseString, const char* state = DefaultStateString, const char* commands = DefaultCommandsString);
  };

  static const Strings DefaultStrings;
  static const Strings EnableDisableStrings;

  /***
   * Mit dieser Methode sind die Strings, die den Datei-Inhalt darstellen konfigurierbar.
   *
   * Die beiden Zustandsbeschreibungen der Variable sind als True-String und als False-String einstellbar,
   * sowie die beiden Präfix-Text vor der Zustands-Angabe und vor der Ausgabe der möglichen Kommandos.
   *
   * Wird der trueString bzw. der falseString mit 0 angegeben, wird standardmäßig "true" bzw. "false" eingesetzt.
   *
   * Wird der statusPrefix mit 0 angegeben, wird ein leerer Präfix-String ausgegeben.
   *
   * Wird der commandPräfix mit 0 angegeben, wird die List der gülitgen Kommandos nicht ausgegeben.
   *
   * @param trueString
   * @param falseString
   * @param statusPrefix
   * @param commandPrefix
   */
  void setStrings(const Strings& strings);
  void setUpdateHandler(const Callback<bool, bool&>* handler); ///< the given handler will be executed on opening the file, until it returns true
  void setChangeHandler(const Callback<bool, bool&>* handler); ///< the given handler will be executed once on closing the file

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
  bool& variable;
  const Strings* string;

  const Callback<bool, bool&>* updateHandler;
  const Callback<bool, bool&>* changeHandler;

  struct IoBuffer: public Npfs::IoState
  {
    const char* readValue;
    unsigned readOffset;
    unsigned writeOffset;
    struct _Flags
    {
      bool writeTrueOk:1;
      bool writeFalseOk:1;
      bool isWritten:1;
    } flag;
  };

  static MemoryManager* mm;

};

inline
void BoolStringFile::useMemory(MemoryManager* mem)
{
  mm = mem;
}

inline
void BoolStringFile::setStrings(const Strings& strings)
{
  string = &strings;
}

inline
void BoolStringFile::setUpdateHandler(const Callback<bool, bool&>* handler)
{
  updateHandler = handler;
}

inline
void BoolStringFile::setChangeHandler(const Callback<bool, bool&>* handler)
{
  changeHandler = handler;
}

inline
constexpr
BoolStringFile::Strings::Strings(const char* active, const char* passive, const char* enable, const char* disable, const char* state, const char* commands)
: trueString(active ? active : DefaultTrueString),
  falseString(passive ? passive : DefaultFalseString),
  setTrueCommand(enable ? enable : DefaultSetTrueString),
  setFalseCommand(disable ? disable : DefaultSetFalseString),
  statusPrefix(state),
  commandsPrefix(commands)
{}

}

#endif /* BOOLSTRINGFILEFILE_H_ */
