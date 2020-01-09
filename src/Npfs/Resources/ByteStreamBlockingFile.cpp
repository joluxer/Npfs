/*
 * ByteStreamBlockingFile.cpp
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#include "ByteStreamBlockingFile.h"

namespace Npfs
{

MemoryManager* ByteStreamBlockingFile::mm = 0;

ByteStreamBlockingFile::ByteStreamBlockingFile(const char* filename, Npfs::Directory* parent, ByteStream* _write, ByteStream* _read, uint16_t umask)
: Resource(filename, parent), writeStream(_write), readStream(_read)
{
  // deny all accesses
  stat.mode &= ~0777;

  if (readStream)
  {
    // file is readable
    stat.mode |= 0444 & ~umask;
  }

  if (writeStream)
  {
    // file is writable
    stat.mode |= 0222 & ~umask;
  }

  // hier funktioniert kein seek, also offsets ignorieren (append mode)
  stat.setMode(stat.mode | Npfs::Dmappend);
  stat.length = 0;  // Größe ist Null, weil es ein Device ist...
}

//ByteStreamBlockingFile::~ByteStreamBlockingFile()
//{
//  // Auto-generated destructor stub
//}

ByteStreamBlockingFile::ResultMessage ByteStreamBlockingFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = OpSuccess;

  return result;
}

ByteStreamBlockingFile::ResultMessage ByteStreamBlockingFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)   // in count will be returned the actual read amount of data, the default implementation refuses the action
{
  ResultMessage result = 0;

  if (readStream)
  {
    OpState* state = static_cast<OpState*>(workRef.opState);

    // lies blockierend vom ByteStream: kehre erst mit result != 0 zurück, wenn der Puffer voll ist
    if (!state)
    {
      // erster Aufruf der Sequenz
      unsigned n;

      n = readStream->readBlock(dataBuffer, count);

      if (n >= count)
      {
        result = OpSuccess;
      }
      else
      {
        state = new(mm) OpState;
        state->bytesDone = n;
      }
    }
    else
    {
      if (count > state->bytesDone)
      {
        state->bytesDone += readStream->readBlock(dataBuffer + state->bytesDone, count - state->bytesDone);
      }

      if (count <= state->bytesDone)
      {
        result = OpSuccess;
        delete state;
        state = 0;
      }
    }

    workRef.opState = state;
  }
  else
  {
    count = 0;
    result = OpSuccess;
  }

  return result;
}

ByteStreamBlockingFile::ResultMessage ByteStreamBlockingFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = 0;

  if (writeStream)
  {
    OpState* state = static_cast<OpState*>(workRef.opState);

    // schreib blockierend auf ByteStream: kehre erst mit result != 0 zurück, wenn der Puffer leer ist
    if (!state)
    {
      // erster Aufruf der Sequenz
      unsigned n;

      n = writeStream->writeBlock(dataBuffer, count);

      if (n >= count)
      {
        result = OpSuccess;
      }
      else
      {
        state = new(mm) OpState;
        state->bytesDone = n;
      }
    }
    else
    {
      if (count > state->bytesDone)
      {
        state->bytesDone += writeStream->writeBlock(dataBuffer + state->bytesDone, count - state->bytesDone);
      }

      if (count <= state->bytesDone)
      {
        result = OpSuccess;
        delete state;
        state = 0;
      }
    }

    workRef.opState = state;
  }
  else
  {
    count = 0;
    result = OpSuccess;
  }

  return result;
}

void ByteStreamBlockingFile::flush(Npfs::OpenIoState& workRef)   ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method
{
  if (workRef.opState)
  {
    delete workRef.opState;
  }
}


ByteStreamBlockingFile::ResultMessage ByteStreamBlockingFile::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength)
{
  ResultMessage result = OpSuccess;

  if (0 != newLength)
    result = Npfs::ErrorString::Eperm;

  return result;

}

}
