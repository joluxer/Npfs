/*
 * ByteStreamFile.cpp
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#include "ByteStreamFile.h"

namespace Npfs
{

ByteStreamFile::ByteStreamFile(const char* filename, Npfs::Directory* parent, ByteStream* output, ByteStream* input, uint16_t umask, bool blockOnZeroInput)
: Resource(filename, parent), outputStream(output), inputStream(input), blockOnZeroInput(blockOnZeroInput)
{
  // deny all accesses
  stat.mode &= ~0777;

  if (inputStream)
  {
    // file is readable
    stat.mode |= 0444 & ~umask;
  }

  if (outputStream)
  {
    // file is writable
    stat.mode |= 0222 & ~umask;
  }

  // hier funktioniert kein seek, also offsets ignorieren (append mode)
  stat.setMode(stat.mode | Npfs::Dmappend);
  stat.length = 0;  // Größe ist Null, weil es ein Device ist...
}

//ByteStreamFile::~ByteStreamFile()
//{
//  // Auto-generated destructor stub
//}

ByteStreamFile::ResultMessage ByteStreamFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = OpSuccess;

  return result;
}

ByteStreamFile::ResultMessage ByteStreamFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)   // in count will be returned the actual read amount of data, the default implementation refuses the action
{
  ResultMessage result = 0;

  if (inputStream)
  {
    unsigned n;

    n = inputStream->readBlock(dataBuffer, count);

    if (n > 0)
    {
      count = n;
      result = OpSuccess;
    }
    else if (!blockOnZeroInput)
    {
      count = 0;
      result = ErrorString::Eblocked;
    }
  }
  else
  {
    count = 0;
    result = OpSuccess;
  }

  return result;
}

ByteStreamFile::ResultMessage ByteStreamFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = OpSuccess;

  if (outputStream)
  {
    count = outputStream->writeBlock(dataBuffer, count);
  }
  else
  {
    count = 0;
    result = OpSuccess;
  }

  return result;
}

void ByteStreamFile::flush(Npfs::OpenIoState& workRef)   ///< cancels the referenced I/O operation immediately, the server forgets about the pending operation (and tag) after return of this method
{
  if (workRef.opState)
  {
    delete workRef.opState;
  }
}


ByteStreamFile::ResultMessage ByteStreamFile::trunc(Npfs::BlockingOpState* &opRef, uint64_t newLength)
{
  ResultMessage result = OpSuccess;

  if (0 != newLength)
    result = Npfs::ErrorString::Eperm;

  return result;
}

}
