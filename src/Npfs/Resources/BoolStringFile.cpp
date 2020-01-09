/*
 * BoolStringFile.cpp
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#include "BoolStringFile.h"

#include <assert.h>
#include <string.h>

namespace Npfs
{

MemoryManager* BoolStringFile::mm = 0;

const char BoolStringFile::DefaultTrueString[] = "true";
const char BoolStringFile::DefaultFalseString[] = "false";
const char BoolStringFile::DefaultSetTrueString[] = "true";
const char BoolStringFile::DefaultSetFalseString[] = "false";
const char BoolStringFile::DefaultStateString[] = "state: ";
const char BoolStringFile::DefaultCommandsString[] = "commands: ";

const BoolStringFile::Strings BoolStringFile::DefaultStrings;
const BoolStringFile::Strings BoolStringFile::EnableDisableStrings = {"enabled", "disabled", "enable", "disable"};

BoolStringFile::BoolStringFile(const char* filename, Npfs::Directory* parent, bool& _variable, uint16_t umask)
: Resource(filename, parent), variable(_variable),
  string(&DefaultStrings),
  updateHandler(0), changeHandler(0)
{
  // set access mode
  stat.mode &= ~0777;
  stat.mode |= 0666 & ~umask;

  // hier funktioniert kein seek, also offsets ignorieren (append mode)
  stat.setMode(stat.mode | Npfs::Dmappend);
//  stat.setMode(stat.mode);
  stat.length = 0;  // Größe ist Null
}

//BoolStringFile::~BoolStringFile()
//{
//  // Auto-generated destructor stub
//}

BoolStringFile::ResultMessage BoolStringFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = 0;
  assert(0 != mm);

  if (!updateHandler || updateHandler->execute(variable))
  {
    IoBuffer* ioRef = new(mm) IoBuffer;

    if (!handleNoMemory(ioRef, result))
    {
      workRef.ioState = ioRef;
      ioRef->readValue = variable ? string->trueString : string->falseString;
      ioRef->readOffset = 0;
      ioRef->writeOffset = 0;
      ioRef->flag.isWritten = false;
      ioRef->flag.writeFalseOk = true;
      ioRef->flag.writeTrueOk = true;

      result = OpSuccess;
    }
    else
    {
      result = NoMemory;
    }
  }

  return result;
}

BoolStringFile::ResultMessage BoolStringFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)   // in count will be returned the actual read amount of data, the default implementation refuses the action
{
  unsigned l,i;
  const char* str;
  unsigned bufferLen = count;
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  const char * const contentString[] =
  {
      string->statusPrefix ? string->statusPrefix : "",
      ioRef->readValue,
      (stat.mode & 0444) && string->commandsPrefix ? "\n\n" : "\n",
      (stat.mode & 0444) && string->commandsPrefix ? string->commandsPrefix : 0,
      string->setTrueCommand,
      ", ",
      string->setFalseCommand,
      "\n",
      0
  };

  count = 0;

  offset = ioRef->readOffset;

  for (i = 0; contentString[i]; i++)
  {
    str = contentString[i];
    l = strlen(str);

    if (offset < l)
    {
      str += offset;
      l -= offset;

      if (l > bufferLen)
      {
        l = bufferLen;
      }

      memcpy(dataBuffer, str, l);

      dataBuffer += l;
      count += l;
      bufferLen -= l;

      if (0 == bufferLen)
      {
        break;
      }
    }
    else
    {
      offset -= l;
    }
  }

  ioRef->readOffset += count;

  return result;

}

BoolStringFile::ResultMessage BoolStringFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  unsigned n;
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);

  offset = ioRef->writeOffset;

  ioRef->flag.isWritten = true;

  // Algorithmus: trueString und falseString parallel durchgehen und in den Flags vermerken, ob der String passt
  if (offset > strlen(string->setTrueCommand))
  {
    ioRef->flag.writeTrueOk = false;
  }

  if (offset > strlen(string->setFalseCommand))
  {
    ioRef->flag.writeFalseOk = false;
  }

  for (n = 0; n < count; n++)
  {
    // ein Zeilenwechsel bricht die Prüfung und das Einlesen ab
    if (('\n' != dataBuffer[n]) && ('\r' != dataBuffer[n]))
    {
      if (ioRef->flag.writeTrueOk)
      {
        ioRef->flag.writeTrueOk = (string->setTrueCommand[offset + n] == dataBuffer[n]);
      }

      if (ioRef->flag.writeFalseOk)
      {
        ioRef->flag.writeFalseOk = (string->setFalseCommand[offset + n] == dataBuffer[n]);
      }
    }
    else
    {
      stat.qid.version++;
      break;
    }

    // Erfolgsprüfung: einer der beiden Strings muss mindestens getroffen werden, evtl beide, Exclusivität wird erst im close() geprüft
    if (!(ioRef->flag.writeFalseOk || ioRef->flag.writeTrueOk))
    {
      result = ErrorString::Enotimpl;
      break;
    }
    else
    {
      stat.qid.version++;
    }
  }

  return result;
}

void BoolStringFile::flush(Npfs::OpenIoState& workRef)   ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method
{
  if (workRef.ioState)
  {
    delete workRef.ioState;
    workRef.ioState = 0;
  }
}

BoolStringFile::ResultMessage BoolStringFile::close(Npfs::OpenIoState& workRef)
{
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);

  if (ioRef->flag.isWritten)
  {
    // nur Variable setzen, wenn exact einer der beiden Strings gesendet wurde, sonst die Operation verwerfen
    if (ioRef->flag.writeFalseOk xor ioRef->flag.writeTrueOk)
    {
      variable = ioRef->flag.writeTrueOk; // nachdem die beiden Flags eine exacte Invertierung von einander sind, enthält das true-Flag den neuen Wert

      if (changeHandler)
        changeHandler->notify(variable);
    }
  }

  delete ioRef;

  workRef.ioState = ioRef = 0;

  return OpSuccess;
}

BoolStringFile::ResultMessage BoolStringFile::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength)
{
  ResultMessage result = OpSuccess;

  if (0 != newLength)
    result = Npfs::ErrorString::Eperm;

  return result;
}

}
