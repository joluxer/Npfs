/*
 * IoPinFile.cpp
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#include "IoPinFile.h"

#include <assert.h>
#include <string.h>

namespace Npfs
{

MemoryManager* IoPinFile::mm = 0;

const char IoPinFile::DefaultActiveString[] = "active";
const char IoPinFile::DefaultPassivString[] = "passive";
const char IoPinFile::DefaultStateString[] = "state: ";
const char IoPinFile::DefaultCommandsString[] = "commands: ";

const IoPinFile::Strings IoPinFile::DefaultStrings;
const Npfs::IoPinFile::Strings IoPinFile::OnOffStrings = {"on", "off"};

IoPinFile::IoPinFile(const char* filename, Npfs::Directory* parent, IoPin& _ioPin, uint16_t umask)
: Resource(filename, parent), myIoPin(_ioPin), string(&DefaultStrings),
  changeHandler(0)
{
  // set access mode
  stat.mode &= ~0777;
  stat.mode |= 0666 & ~umask;

  // hier funktioniert kein seek, also offsets ignorieren (append mode)
  stat.setMode(stat.mode | Npfs::Dmappend);
//  stat.setMode(stat.mode);
  stat.length = 0;  // Größe ist Null
}

//IoPinFile::~IoPinFile()
//{
//  // Auto-generated destructor stub
//}

IoPinFile::IoBuffer* IoPinFile::makeIoBuffer()
{
  assert(0 != mm);

  return new(mm) IoBuffer;
}

IoPinFile::ResultMessage IoPinFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = 0;

  IoBuffer* ioRef = makeIoBuffer();

  if (!handleNoMemory(ioRef, result))
  {
    workRef.ioState = ioRef;
    if (blockRead(*ioRef))
      ioRef->readValue = 0;
    else
      ioRef->readValue = readValue(*ioRef);
    ioRef->readOffset = 0;
    ioRef->writeOffset = 0;
    ioRef->flag.isWritten = false;
    ioRef->flag.writePassiveOk = true;
    ioRef->flag.writeActiveOk = true;

    result = OpSuccess;
  }
  else
  {
    result = NoMemory;
  }

  return result;
}

IoPinFile::ResultMessage IoPinFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)   // in count will be returned the actual read amount of data, the default implementation refuses the action
{
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);
  ResultMessage result = 0;

  if (!ioRef->readValue)
  {
    if (releaseRead(*ioRef))
      ioRef->readValue = readValue(*ioRef);
  }

  if (ioRef->readValue)
  {
    unsigned l,i;
    const char* str;
    unsigned bufferLen = count;
    const char * const contentString[] =
    {
        string->statePrefix ? string->statePrefix : "",
        ioRef->readValue,
        (stat.mode & 0444) && string->commandsPrefix ? "\n\n" : "\n",
        (stat.mode & 0444) && string->commandsPrefix ? string->commandsPrefix : 0,
        string->activeString,
        ", ",
        string->passiveString,
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

    result = OpSuccess;
  }

  return result;

}

IoPinFile::ResultMessage IoPinFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  unsigned n;
  ResultMessage result = OpSuccess;
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);

  offset = ioRef->writeOffset;

  ioRef->flag.isWritten = true;

  // Algorithmus: activeString und passiveString parallel durchgehen und in den Flags vermerken, ob der String passt
  if (offset > strlen(string->activeString))
  {
    ioRef->flag.writeActiveOk = false;
  }

  if (offset > strlen(string->passiveString))
  {
    ioRef->flag.writePassiveOk = false;
  }

  for (n = 0; n < count; n++)
  {
    // ein Zeilenwechsel bricht die Prüfung und das Einlesen ab
    if (('\n' != dataBuffer[n]) && ('\r' != dataBuffer[n]))
    {
      if (ioRef->flag.writeActiveOk)
      {
        ioRef->flag.writeActiveOk = (string->activeString[offset + n] == dataBuffer[n]);
      }

      if (ioRef->flag.writePassiveOk)
      {
        ioRef->flag.writePassiveOk = (string->passiveString[offset + n] == dataBuffer[n]);
      }
    }
    else
    {
      stat.qid.version++;
      break;
    }


    // Erfolgsprüfung: einer der beiden Strings muss mindestens getroffen werden, evtl beide, Exclusivität wird erst im close() geprüft
    if (!(ioRef->flag.writePassiveOk || ioRef->flag.writeActiveOk || isWriteAccepted(*ioRef, dataBuffer, count, offset)))
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

void IoPinFile::flush(Npfs::OpenIoState& workRef)   ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method
{
  if (workRef.ioState)
  {
    delete workRef.ioState;
    workRef.ioState = 0;
  }
}

IoPinFile::ResultMessage IoPinFile::close(Npfs::OpenIoState& workRef)
{
  IoBuffer* ioRef = static_cast<IoBuffer*>(workRef.ioState);

  if (ioRef->flag.isWritten)
  {
    // nur Variable setzen, wenn exact einer der beiden Strings gesendet wurde, sonst die Operation verwerfen
    if (ioRef->flag.writePassiveOk xor ioRef->flag.writeActiveOk)
    {
      if (ioRef->flag.writeActiveOk) // nachdem die beiden Flags eine exakte Invertierung von einander sind, enthält das true-Flag den neuen Wert
      {
        myIoPin.activate();
      }
      else
      {
        myIoPin.deactivate();
      }

      if (changeHandler)
        changeHandler->notify(ioRef->flag.writeActiveOk);
    }
  }

  delete ioRef;

  workRef.ioState = ioRef = 0;

  return OpSuccess;
}

IoPinFile::ResultMessage IoPinFile::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength)
{
  ResultMessage result = OpSuccess;

  if (0 != newLength)
    result = Npfs::ErrorString::Eperm;

  return result;
}

bool IoPinFile::blockRead(IoBuffer& workRef)
{
  return false;
}

bool IoPinFile::releaseRead(IoBuffer& workRef)
{
  return true;
}

const char* IoPinFile::readValue(IoBuffer& workRef)
{
  return myIoPin.isActive() ? string->activeString : string->passiveString;
}

bool IoPinFile::isWriteAccepted(IoBuffer& workRef, const unsigned char* buffer, unsigned bufLen, uint64_t offset)
{
  return false;
}

}
