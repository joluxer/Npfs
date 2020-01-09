/*
 * MsgSerDes.cpp
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#include "MsgSerDes.h"

#include <assert.h>

namespace Npfs
{

MsgSerDes::MsgSerDes()
: mm(0), rbyteCount(0)
{}

MsgSerDes::~MsgSerDes()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

uint64_t MsgSerDes::getUI64()
{
  uint64_t ret = 0;
  unsigned char buf[sizeof(ret)];
  unsigned n;

  n = readData(buf, sizeof(buf));
  rbyteCount += n;

  if (sizeof(buf) == n)
  {
    ret = (typeof(ret)) buf[0] |
      ((typeof(ret)) buf[1] << 8) |
      ((typeof(ret)) buf[2] << 16) |
      ((typeof(ret)) buf[3] << 24) |
      ((typeof(ret)) buf[4] << 32) |
      ((typeof(ret)) buf[5] << 40) |
      ((typeof(ret)) buf[6] << 48) |
      ((typeof(ret)) buf[7] << 56);
  }

  return ret;
}

uint32_t MsgSerDes::getUI32()
{
  uint32_t ret = 0;
  unsigned char buf[sizeof(ret)];
  unsigned n;

  n = readData(buf, sizeof(buf));
  rbyteCount += n;

  if (sizeof(buf) == n)
  {
    ret = (typeof(ret))buf[0] | ((typeof(ret))buf[1] << 8) | ((typeof(ret))buf[2] << 16) | ((typeof(ret))buf[3] << 24);
  }

  return ret;
}

uint16_t MsgSerDes::getUI16()
{
  uint16_t ret = 0;
  unsigned char buf[sizeof(ret)];
  unsigned n;

  n = readData(buf, sizeof(buf));
  rbyteCount += n;

  if (sizeof(buf) == n)
  {
    ret = (typeof(ret))buf[0] | ((typeof(ret))buf[1] << 8);
  }

  return ret;
}

uint8_t MsgSerDes::getUI8()
{
  uint8_t ret = 0;
  unsigned char buf[sizeof(ret)];
  unsigned n;

  n = readData(buf, sizeof(buf));
  rbyteCount += n;

  if (sizeof(buf) == n)
  {
    ret = (typeof(ret))buf[0];
  }

  return ret;
}

void MsgSerDes::get(NpStr& str)
{
  str.len = getUI16();
  if (str.len)
  {
    str.str = (char*) mm->allocate(str.len);
    assert(0 != str.str);
    if (str.str)
    {
      unsigned n = readData((unsigned char*)str.str, str.len);
      rbyteCount += n;
      assert(str.len == n);
      if (str.len != n)
      {
        mm->release(str.str);
        str.str = 0;
        str.len = 0;
      }
    }
    else
    {
      str.len = 0;
    }
  }
}

void MsgSerDes::release(Npfs::NpStr& string)
{
  mm->release(string.str);
  string.str = 0;
}

void MsgSerDes::get(unsigned char* &buffer, uint32_t count)
{
  buffer = (unsigned char*) mm->allocate(count);

  assert(buffer != 0);

  if (buffer)
  {
    unsigned n = readData(buffer, count);
    rbyteCount += n;
    assert(n == count);
    if (count != n)
    {
      mm->release(buffer);
      buffer = 0;
    }
  }
}

void MsgSerDes::release(unsigned char* &buffer)
{
  mm->release(buffer);
  buffer = 0;
}

void MsgSerDes::get(NpQid& qid)
{
  qid.type = getUI8();
  qid.version = getUI32();
  qid.path = getUI64();
}

void MsgSerDes::putUI64(uint64_t val)
{
  unsigned char buf[sizeof(val)];

  buf[0] = val;
  buf[1] = val >> 8;
  buf[2] = val >> 16;
  buf[3] = val >> 24;
  buf[4] = val >> 32;
  buf[5] = val >> 40;
  buf[6] = val >> 48;
  buf[7] = val >> 56;

  unsigned n = sendData(buf, sizeof(buf));
  n = n; // make compiler happy of unused variable
  assert(n == sizeof(buf));
}

void MsgSerDes::putUI32(uint32_t val)
{
  unsigned char buf[sizeof(val)];

  buf[0] = val;
  buf[1] = val >> 8;
  buf[2] = val >> 16;
  buf[3] = val >> 24;

  unsigned n = sendData(buf, sizeof(buf));
  n = n; // make compiler happy of unused variable
  assert(n == sizeof(buf));
}

void MsgSerDes::putUI16(uint16_t val)
{
  unsigned char buf[sizeof(val)];

  buf[0] = val;
  buf[1] = val >> 8;

  unsigned n = sendData(buf, sizeof(buf));
  n = n; // make compiler happy of unused variable
  assert(n == sizeof(buf));
}

void MsgSerDes::putUI8(uint8_t val)
{
  unsigned char buf[sizeof(val)];

  buf[0] = val;

  unsigned n = sendData(buf, sizeof(buf));
  n = n; // make compiler happy of unused variable
  assert(n == sizeof(buf));
}

void MsgSerDes::put(const NpStr& string)
{
  putUI16(string.len);
  unsigned n = sendData((unsigned char*)string.str, string.len);
  n = n; // make compiler happy of unused variable
  assert(n == string.len);
}

void MsgSerDes::put(const NpQid& qid)
{
  putUI8(qid.type);
  putUI32(qid.version);
  putUI64(qid.path);
}

void MsgSerDes::put(const unsigned char* buffer, uint32_t count)
{
  unsigned sent = sendData(buffer, count);
  sent = sent; // make compiler happy of unused variable
  assert(sent == count);
}

void MsgSerDes::put(const NpStat& stat)
{
  putUI16(stat.statSize());
  putUI16(stat.type);
  putUI32(stat.dev);
  put(stat.qid);
  putUI32(stat.mode);
  putUI32(stat.atime);
  putUI32(stat.mtime);
  putUI64(stat.length);

  put(stat.name);
  put(stat.uid);
  put(stat.gid);
  put(stat.muid);
}

}
