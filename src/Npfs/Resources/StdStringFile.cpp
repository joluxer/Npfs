/*
 * StdStringFile.cpp
 *
 */

#include "StdStringFile.h"

#include <string.h>

#include <assert.h>

namespace Npfs
{

StdStringFile::StdStringFile(const char* filename, Npfs::Directory* parent, std::string& storage, uint16_t umask)
: Npfs::Resource(filename, parent), fileData(storage),
  updateHandler(0), changeHandler(0), truncHandler(0), modifiedHandler(0)
{
  // deny all accesses
  stat.mode &= ~0777;

  stat.mode |= 0666 & ~umask;
  stat.setMode(stat.mode);
}

//StdStringFile::~StdStringFile()
//{
//  // Auto-generated destructor stub
//}

void * StdStringFile::IoRefState::operator new(size_t size) throw()
{
  return new unsigned char[size];
}

void StdStringFile::IoRefState::operator delete(void *mem) throw()
{
  delete[] (unsigned char*)mem;
}

Npfs::Resource::ResultMessage StdStringFile::open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit)
{
  ResultMessage result = 0;

  setNewSize(fileData.length());

  if (!updateHandler || updateHandler->execute(fileData, stat.length))
  {
    IoRefState* ioRef = new IoRefState;

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

Npfs::Resource::ResultMessage StdStringFile::read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset)  // in count will be returned the actual read amount of data
{
  ResultMessage result = OpSuccess;

  setNewSize(fileData.length());

  if (stat.length <= offset)
  {
    // nichts kann gelesen werden
    count = 0;
  }
  // sonst ist die volle Menge lesbar

  if (count)
    count = fileData.copy((char*) dataBuffer, count, offset);

  return result;
}

Npfs::Resource::ResultMessage StdStringFile::write(Npfs::OpenIoState& workRef, const unsigned char* dataBuffer, uint32_t& count, uint64_t offset)
{
  ResultMessage result = OpSuccess;
  IoRefState* ioRef = static_cast<IoRefState*>(workRef.ioState);


  if (fileData.length() < (offset + count))
  {
    fileData.reserve(offset + count);
    fileData.append((offset + count) - fileData.length(), 0);
    ioRef->isWritten = true;
  }

  if (count)
  {
    fileData.replace(offset, count, (const char*) dataBuffer, count);

    // Datei-Laenge bearbeiten
    setNewSize(fileData.length());
    stat.qid.version++;
    ioRef->isWritten = true;
    if (modifiedHandler)
    {
      uint32_t countCopy = count; // needed, as the handler is allowed to modify the value, but we want to ignore these changes.
      uint64_t offsetCopy = offset; // needed, as the handler is allowed to modify the value, but we want to ignore these changes.
      modifiedHandler->notify(fileData, countCopy, offsetCopy);
    }
  }

  return result;
}

void StdStringFile::flush(Npfs::OpenIoState& workRef)
{
  // intentionally left empty
}

Npfs::Resource::ResultMessage StdStringFile::close(Npfs::OpenIoState& workRef)
{
  ResultMessage result = OpSuccess;
  IoRefState* ioRef = static_cast<IoRefState*>(workRef.ioState);

  setNewSize(fileData.length());

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

Npfs::Resource::ResultMessage StdStringFile::trunc(Npfs::BlockingOpState* &workRef, uint64_t newLength)
{
  ResultMessage result = 0;

    if (truncHandler)
    {
      if (truncHandler->notify(fileData, newLength))
      {
        fileData.erase(newLength);
        setNewSize(fileData.length());
        result = OpSuccess;
      }
    }
    else
    {
      fileData.erase(newLength);
      setNewSize(fileData.length());
      result = OpSuccess;
    }

  return result;
}

const NpStat& StdStringFile::getStat()
{
  setNewSize(fileData.length());

  return Resource::getStat();
}

}
