/*
 * RamFile.cpp
 *
 */

#include "RamFile.h"

#include <string.h>

#include <assert.h>

namespace Npfs
{

MemoryManager* RamFile::mm = 0;

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, unsigned char* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData(fileArea), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(UnalignedZeroFilled),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

//RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, uint8_t* fileArea, uint16_t umask)
//: Npfs::Resource(filename, parent), fileData(reinterpret_cast<unsigned char*>(fileArea)), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint8AlignedZeroFilled),
//  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
//{
//  // deny all accesses
//  stat.mode &= ~0777;
//
//  stat.mode |= 0666 & ~umask;
//  stat.setMode(stat.mode);
//}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, uint16_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData(reinterpret_cast<unsigned char*>(fileArea)), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint16AlignedZeroFilled),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, uint32_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData(reinterpret_cast<unsigned char*>(fileArea)), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint32AlignedZeroFilled),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, uint64_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData(reinterpret_cast<unsigned char*>(fileArea)), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint64AlignedZeroFilled),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, volatile uint8_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData((unsigned char*)fileArea), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint8AlignedSparse),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, volatile uint16_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData((unsigned char*)fileArea), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint16AlignedSparse),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, volatile uint32_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData((unsigned char*)fileArea), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint32AlignedSparse),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

RamFile::RamFile(const char* filename, Npfs::Directory* parent, unsigned maxFileSize, volatile uint64_t* fileArea, uint16_t umask)
: Npfs::Resource(filename, parent), fileData((unsigned char*)fileArea), maxDataSize(maxFileSize * sizeof(fileArea[0])), memAccess(Uint64AlignedSparse),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

//RamFile::~RamFile()
//{
//  // Auto-generated destructor stub
//}


Npfs::Resource::ResultMessage RamFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = 0;
  assert(0 != mm);

  if (!updateHandler || updateHandler->execute(fileData, stat.length))
  {
    assert(stat.length <= maxDataSize);

    IoRefState* ioRef = new(mm) IoRefState;

    if (!handleNoMemory(ioRef, result))
    {
      ioRef->isWritten = false;
      workRef.ioState = ioRef;

      result = OpSuccess;
    }
    else
    {
      result = NoMemory;
    }
  }

  return result;
}

void RamFile::copyUnaligned(unsigned char *dataBuffer, const unsigned char *fileData, uint32_t count)
{
  memcpy(dataBuffer, fileData, count);
}

template<typename T>
void RamFile::copyFromFileAlignedFor(unsigned char *dataBuffer, const unsigned char *fileData, uint32_t count)
{
  // die Mimik hier ist Endian-fest, da nur das Lesen und Schreiben der Speicherzelle
  // in Bitbreiten größer 8 Bit ausgeführt wird, der temporäre Puffer jedoch wieder Byteweise bearbeitet wird.
  // Daher hat der tmpBuffer den gleichen Inhalt wie fileData

  const T *tmpPtr;
  T tmpVal;
  unsigned tmpOffset;
  uint32_t alignedCount;
  uint32_t offset = 0;
  const unsigned incr = sizeof(T);
  size_t mask = incr - 1;
  unsigned char *const tmpBuffer = reinterpret_cast<unsigned char*>(&tmpVal);

  // Anfangsoffset kompensieren
  if (((size_t)fileData & mask) != 0)
  {
    tmpOffset = (size_t)fileData % incr;

    tmpPtr = reinterpret_cast<const T*>((size_t)fileData & ~mask);
    tmpVal = *tmpPtr;

    for (; (tmpOffset < incr) && (offset < count); offset++, tmpOffset++)
    {
      dataBuffer[offset] = tmpBuffer[tmpOffset];
    }
  }

  alignedCount = count - offset;

  // aligned Bereich ermitteln
  alignedCount &= ~mask;

  // kopieren
  for (; offset < alignedCount; offset += incr)
  {
    tmpPtr = reinterpret_cast<const T*>(fileData + offset);
    tmpVal = *tmpPtr;

    for (tmpOffset = 0; tmpOffset < incr; tmpOffset++)
    {
      dataBuffer[offset + tmpOffset] = tmpBuffer[tmpOffset];
    }
  }

  // Restdaten kopieren
  if (offset < count)
  {
    tmpPtr = reinterpret_cast<const T*>(fileData + offset);
    tmpVal = *tmpPtr;

    for (tmpOffset = 0; (offset < count) && (tmpOffset < incr); offset++, tmpOffset++)
    {
      dataBuffer[offset] = tmpBuffer[tmpOffset];
    }
  }
}

template<typename T>
void RamFile::copyToFileAlignedFor(volatile unsigned char *fileData, const unsigned char *dataBuffer, uint32_t count)
{
  // die Mimik hier ist Endian-fest, da nur das Lesen und Schreiben der Speicherzelle
  // in Bitbreiten größer 8 Bit ausgeführt wird, der temporäre Puffer jedoch wieder Byteweise bearbeitet wird.
  // Daher hat der tmpBuffer den gleichen Inhalt wie fileData

  volatile T *tmpPtr;
  T tmpVal;
  unsigned tmpOffset;
  uint32_t alignedCount;
  uint32_t offset = 0;
  const unsigned incr = sizeof(T);
  size_t mask = incr - 1;
  unsigned char *const tmpBuffer = reinterpret_cast<unsigned char*>(&tmpVal);

  // Anfangsoffset kompensieren
  if (((size_t)fileData & mask) != 0)
  {
    tmpOffset = (size_t)fileData % incr;

    // read-modify-write: read stage
    tmpPtr = reinterpret_cast<volatile T*>((size_t)fileData & ~mask);
    tmpVal = *tmpPtr;

    // read-modify-write: modify stage
    for (; (tmpOffset < incr) && (offset < count); offset++, tmpOffset++)
    {
      tmpBuffer[tmpOffset] = dataBuffer[offset];
    }

    // read-modify-write: write stage
    *tmpPtr = tmpVal;
  }

  alignedCount = count - offset;

  // aligned Bereich ermitteln
  alignedCount &= ~mask;

  // kopieren
  for (; offset < alignedCount; offset += incr)
  {
    for (tmpOffset = 0; tmpOffset < incr; tmpOffset++)
    {
      tmpBuffer[tmpOffset] = dataBuffer[offset + tmpOffset];
    }

    tmpPtr = reinterpret_cast<volatile T*>(fileData + offset);
    *tmpPtr = tmpVal;
  }

  // Restdaten kopieren
  if (offset < count)
  {
    // read-modify-write: read stage
    tmpPtr = reinterpret_cast<volatile T*>((size_t)fileData + offset);
    tmpVal = *tmpPtr;

    // read-modify-write: modify stage
    for (tmpOffset = 0; (offset < count) && (tmpOffset < incr); offset++, tmpOffset++)
    {
      tmpBuffer[tmpOffset] = dataBuffer[offset];
    }

    // read-modify-write: write stage
    *tmpPtr = tmpVal;
  }
}

Npfs::Resource::ResultMessage RamFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)  // in count will be returned the actual read amount of data
{
  ResultMessage result = OpSuccess;

  if (!fileData || (stat.length <= offset))
  {
    // nichts kann gelesen werden
    count = 0;
  }
  else if (stat.length <= (offset + count))
  {
    // Lese-Menge begrenzen
    count = stat.length - offset;
  }
  // sonst ist die volle Menge lesbar

  if (count)
  {
    switch (memAccess)
    {
    case UnalignedZeroFilled:
    case UnalignedSparse:
      copyUnaligned(dataBuffer, fileData + offset, count);
      break;
    case Uint8AlignedZeroFilled:
    case Uint8AlignedSparse:
      copyFromFileAlignedFor<uint8_t>(dataBuffer, fileData + offset, count);
      break;
    case Uint16AlignedZeroFilled:
    case Uint16AlignedSparse:
      copyFromFileAlignedFor<uint16_t>(dataBuffer, fileData + offset, count);
      break;
    case Uint32AlignedZeroFilled:
    case Uint32AlignedSparse:
      copyFromFileAlignedFor<uint32_t>(dataBuffer, fileData + offset, count);
      break;
    case Uint64AlignedZeroFilled:
    case Uint64AlignedSparse:
      copyFromFileAlignedFor<uint64_t>(dataBuffer, fileData + offset, count);
      break;
    }
  }

  return result;
}

Npfs::Resource::ResultMessage RamFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = OpSuccess;
  IoRefState* ioRef = static_cast<IoRefState*>(workRef.ioState);

  if (!fileData || (maxDataSize <= offset))
  {
    // nichts kann geschrieben werden
    count = 0;
    result = Npfs::ErrorString::Enospace;
  }
  else if (maxDataSize <= (offset + count))
  {
    // Schreib-Menge begrenzen
    count = maxDataSize - offset;
  }
  // sonst ist die volle Menge schreibbar

  if (count)
  {
    // aktuelle Datenmenge ggf. bis zum neuen Offset mit Null auffuellen
    if (stat.length <= offset)
    {
      switch (memAccess)
      {
      case UnalignedZeroFilled:
      case Uint8AlignedZeroFilled:
      case Uint16AlignedZeroFilled:
      case Uint32AlignedZeroFilled:
      case Uint64AlignedZeroFilled:
        memset(fileData + stat.length, 0, offset - stat.length);
        break;
      default:
        break;
      }
    }
    switch (memAccess)
    {
    case UnalignedZeroFilled:
    case UnalignedSparse:
      copyUnaligned(fileData + offset, dataBuffer, count);
      break;
    case Uint8AlignedZeroFilled:
    case Uint8AlignedSparse:
      copyToFileAlignedFor<uint8_t>(fileData + offset, dataBuffer, count);
      break;
    case Uint16AlignedZeroFilled:
    case Uint16AlignedSparse:
      copyToFileAlignedFor<uint16_t>(fileData + offset, dataBuffer, count);
      break;
    case Uint32AlignedZeroFilled:
    case Uint32AlignedSparse:
      copyToFileAlignedFor<uint32_t>(fileData + offset, dataBuffer, count);
      break;
    case Uint64AlignedZeroFilled:
    case Uint64AlignedSparse:
      copyToFileAlignedFor<uint64_t>(fileData + offset, dataBuffer, count);
      break;
    }
    // Datei-Laenge bearbeiten
    if (stat.length < (offset + count))
    {
      stat.length = (offset + count);
    }
    stat.qid.version++;
    ioRef->isWritten = true;
    if (modifiedHandler)
    {
      uint32_t countCopy = count; // needed, as the handler is allowed to modify the value, but we want to ignore these changes.
      modifiedHandler->notify(fileData + offset, countCopy);
    }
  }

  return result;
}

void RamFile::flush(Npfs::OpenIoState& workRef)
{
  // intentionally left empty
}

Npfs::Resource::ResultMessage RamFile::close(Npfs::OpenIoState& workRef)
{
  ResultMessage result = OpSuccess;
  IoRefState* ioRef = static_cast<IoRefState*>(workRef.ioState);

  if (ioRef->isWritten)
  {
    if (!changeHandler || changeHandler->notify(fileData, stat.length))
    {
      delete ioRef;
      workRef.ioState = ioRef = 0;
    }
    else
    {
      result = 0;
    }
  }
  else
  {
    delete ioRef;
    workRef.ioState = ioRef = 0;
  }

  return result;
}

Npfs::Resource::ResultMessage RamFile::trunc(Npfs::BlockingOpState* &workRef, uint64_t newLength)
{
  ResultMessage result = 0;

  if (newLength <= maxDataSize)
  {
    stat.length = newLength;
    if (truncHandler)
    {
      if (truncHandler->notify(fileData, stat.length))
      {
        result = OpSuccess;
      }
    }
    else
    {
      result = OpSuccess;
    }
  }
  else
  {
    result = Npfs::ErrorString::Enospace;
  }

  return result;
}

}
